
#include <stdio.h>
#include <stdlib.h>

#include "texture.h"

typedef unsigned char uchar;
typedef unsigned short ushort;

typedef struct
{
    uchar magic; // 0x0a
    uchar version;
    uchar encoding; // 0x01
    uchar bitsperpixel;

    ushort xmin;
    ushort ymin;
    ushort xmax;
    ushort ymax;
    ushort hres;
    ushort vres;

    uchar palette[48];
    uchar reserved;
    uchar numofplanes;

    ushort bytesperline;
    ushort palettetype;
    ushort hsize;
    ushort vsize;

    uchar filler[54];
} pcxhead_t;

void unload_texture(texture_t* texture)
{
    if (texture->palette) {
        free(texture->palette);
        if (texture->data) {
            free(texture->data);
        }
    }
}

void read_compressed_data(FILE* fp, int size, unsigned char* data)
{
    int i = 0;
    unsigned char j, b;

    fseek(fp, 128, SEEK_SET);

    while (i != size) {
        b = fgetc(fp);
        if (b >= 192) {
            j = b - 192;
            b = fgetc(fp);
            while (j-- > 0) {
                data[i++] = b;
            }
        } else {
            data[i++] = b;
        }
    }
}

void read_palette(FILE* fp, unsigned char* palette)
{
    int i;

    fseek(fp, -768L, SEEK_END);

    for (i = 0; i < 768; i++) {
        palette[i] = fgetc(fp) >> 2;
    }
}

int load_texture(texture_t* texture, char* filename)
{
    pcxhead_t header;
    FILE* fp;
    int i, is_success = 0;

    fp = fopen(filename, "r+b");
    if (fp) {
        fread(&header, sizeof(pcxhead_t), 1, fp);

        if (header.magic == 0x0a && header.bitsperpixel == 8) {
            texture->width = header.xmax - header.xmin + 1;
            texture->height = header.ymax - header.ymin + 1;

            if (texture->width * texture->height < 0xffff) {
                texture->data = (unsigned char *)malloc(texture->width * texture->height * sizeof(unsigned char));
        
                if (texture->data) {
                    texture->palette = (unsigned char *)malloc(768 * sizeof(unsigned char));
        
                    if (texture->palette) {
                        read_compressed_data(fp, texture->width * texture->height, texture->data);
                        read_palette(fp, texture->palette);

                        is_success = 1;
                    }
                }
            }
        }

        fclose(fp);
    }

    if (!is_success) {
        unload_texture(texture);
    }

    return is_success;
}
