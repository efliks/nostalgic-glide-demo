
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "bitmap.h"

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

void unload_bitmap(bitmap_t* bitmap)
{
    if (bitmap->data) {
        free(bitmap->data);
        if (bitmap->palette) {
            free(bitmap->palette);
        }
    }
}

static void read_compressed_data(FILE* fp, int size, unsigned char* data)
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

static void read_palette(FILE* fp, unsigned char* palette)
{
    int i;

    fseek(fp, -768L, SEEK_END);

    for (i = 0; i < 768; i++) {
        palette[i] = fgetc(fp) >> 2;
    }
}

int load_bitmap(bitmap_t* bitmap, const char* filename)
{
    pcxhead_t header;
    FILE* fp;
    int is_success = 0;

    fp = fopen(filename, "r+b");
    if (fp) {
        fread(&header, sizeof(pcxhead_t), 1, fp);

        if (header.magic == 0x0a && header.bitsperpixel == 8) {
            bitmap->width = header.xmax - header.xmin + 1;
            bitmap->height = header.ymax - header.ymin + 1;

            //if (bitmap->width * bitmap->height < 0xffff) {
            if (1 == 1) {
                bitmap->data = (unsigned char *)malloc(bitmap->width * bitmap->height * sizeof(unsigned char));
        
                if (bitmap->data) {
                    bitmap->palette = (unsigned char *)malloc(768 * sizeof(unsigned char));
        
                    if (bitmap->palette) {
                        read_compressed_data(fp, bitmap->width * bitmap->height, bitmap->data);
                        read_palette(fp, bitmap->palette);

                        is_success = 1;
                    }
                }
            }
        }

        fclose(fp);
    }

    if (!is_success) {
        unload_bitmap(bitmap);
    }

    return is_success;
}

static void write_compressed_data(FILE* fp, int size, unsigned char* data)
{
    int i, j;
    unsigned char c, cc, n_found;

    i = 0;
    while (i != size) {
        c = data[i];

        if (c >= 192) {
            cc = 192 | 1;
            fputc(cc, fp);
            fputc(c, fp);
            i++;
            continue;
        }

        n_found = 0;
        for (j = 1; j < 63; j++) {
            if ((i + j) == size)
                break;
            if (data[i + j] == c)
                n_found++;
            else
                break;
        }

        if (n_found) {
            cc = 192 | n_found;
            fputc(cc, fp);
            fputc(c, fp);
            i += n_found;
        } else {
            fputc(c, fp);
            i++;
        }
    }
}

static void write_palette(FILE* fp, unsigned char* palette)
{
    int i;

    fputc(0x0c, fp); // magic number
    for (i = 0; i < 768; i++) {
        fputc(palette[i] << 2, fp);
    }
}

int store_bitmap(bitmap_t* bitmap, const char* filename)
{
    store_bitmap_with_text(bitmap, filename, NULL);
}

int store_bitmap_with_text(bitmap_t* bitmap, const char* filename, char* hiddentext)
{
    FILE* fp;
    int is_success = 0;
    pcxhead_t* header;
    unsigned char myheader[128];

    fp = fopen(filename, "w+b");
    if (fp) {
        memset(myheader, 0, 128);
        header = (pcxhead_t *)myheader;

        header->magic = 0x0a;
        header->version = 5;
        header->encoding = 1;
        header->bitsperpixel = 8;

        header->xmin = 0;
        header->xmax = bitmap->width - 1;
        header->ymin = 0;
        header->ymax = bitmap->height - 1;
        header->hsize = bitmap->width;
        header->vsize = bitmap->height;

        header->bytesperline = bitmap->width;
        header->palettetype = 1;
        header->numofplanes = 1;
        if (hiddentext != NULL) {
            // TODO Handle error if too long
            strcpy((char *)header->filler, hiddentext);
        }

        fwrite(myheader, 128, 1, fp);
 
        write_compressed_data(fp, bitmap->width * bitmap->height, bitmap->data);
        write_palette(fp, bitmap->palette);

        is_success = 1;
    }

    return is_success;
}

int create_empty_bitmap(bitmap_t* bitmap, int size)
{
    bitmap->width = size;
    bitmap->height = size;
    bitmap->data = (unsigned char *)malloc(bitmap->width * bitmap->height * sizeof(unsigned char));
    if (bitmap->data != NULL) {
        bitmap->palette = (unsigned char *)malloc(768 * sizeof(unsigned char));
        if (bitmap->palette != NULL) {
            return 1;
        }

        free(bitmap->data);
    }

    return 0;
}
