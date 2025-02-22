
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <glide.h>

#include "texture.h"

typedef struct
{
    unsigned char r, g, b;
} palette_color_t;

// Copy from 3dfx tlib
static GrTexTable_t texTableType( GrTextureFormat_t format ) 
{
    GrTexTable_t rv = (GrTexTable_t)-1;
    switch( format ) {
    case GR_TEXFMT_YIQ_422:
    case GR_TEXFMT_AYIQ_8422:
        rv = GR_TEXTABLE_NCC0;
        break;
    case GR_TEXFMT_P_8:
    case GR_TEXFMT_AP_88:
        rv = GR_TEXTABLE_PALETTE;
        break;
    }
    return rv;
}

// End of copy from 3dfx tlib

static int load_3dffile(Gu3dfInfo* gu, const char* filename)
{
    int is_good = 0;

    if (gu3dfGetInfo(filename, gu)) {
        gu->data = malloc(gu->mem_required);

        if (gu->data != NULL) {
            if (gu3dfLoad(filename, gu)) {
                is_good = 1;
            }
            else {
                free(gu->data);
            }
        }
    }

    return is_good;
}

static void unload_glide_texture(glidetexture_t* gt)
{
    if (gt->info.data != NULL) {
        free(gt->info.data);
    }
}

static int load_glide_texture(glidetexture_t* gt, const char* filename)
{
    int is_good = 0;

    Gu3dfInfo gu;

    if (load_3dffile(&gu, filename)) {
        gt->is_in_tmu = 0;
        gt->tmu_memory_addr = -1;

        gt->info.smallLod = gu.header.small_lod;
        gt->info.largeLod = gu.header.large_lod;
        gt->info.aspectRatio = gu.header.aspect_ratio;
        gt->info.format = gu.header.format;
        
        // Think to free this memory !
        gt->info.data = gu.data;

        gt->tabletype = texTableType(gt->info.format);

        switch (gt->tabletype) {
        case GR_TEXTABLE_NCC0:
        case GR_TEXTABLE_NCC1:
        case GR_TEXTABLE_PALETTE:
            memcpy(&gt->table, &(gu.table), sizeof(GuTexTable));
            break;
        default:
            break;
        }

        is_good = 1;
    }

    return is_good;
}

static void quantize_palette(palette_color_t* palette, unsigned short* colors)
{
    int i;
    unsigned short red, green, blue, final;
    palette_color_t pc;
    float cf;

    for (i = 0; i < 256; i++) {
        pc = *(palette++);

        cf = (float)(pc.r) / 63. * 255.;
        red = (unsigned short)(cf);

        cf = (float)(pc.g) / 63. * 255.;
        green = (unsigned short)(cf);

        cf = (float)(pc.b) / 63. * 255.;
        blue = (unsigned short)(cf);

        red = (red >> 3) & 0x1f;
        green = (green >> 2) & 0x3f;
        blue = (blue >> 3) & 0x1f;
        final = (red << 11) | (green << 5) | blue;
        *(colors++) = final;
    }
}

static void quantize_to_rgb565(unsigned char* src, palette_color_t* palette, int size, unsigned short* dest)
{
    int i;
    unsigned short colors[256];

    quantize_palette(palette, colors);

    for (i = 0; i < size; i++) {
        *(dest++) = colors[*(src++)];
    }
}

static int load_bitmap_texture(glidetexture_t* gt, const char* filename)
{
    bitmap_t tb;
    GrLOD_t level;
    int is_success;

    is_success = load_bitmap(&tb, filename);

    if (is_success) {
        is_success = (tb.width == tb.height && 
            (tb.width == 128 || 
            tb.width == 256)) ? 1 : 0;

        if (is_success) {
            gt->info.data = malloc(tb.width * tb.height * sizeof(unsigned short));
            is_success = (gt->info.data != NULL) ? 1 : 0;

            if (is_success) {
                switch (tb.width) {
                case 128:
                    level = GR_LOD_128;
                    break;
                case 256:
                    level = GR_LOD_256;
                    break;
                default:
                    break;
                }
                gt->info.smallLod = level;
                gt->info.largeLod = level;
                gt->info.aspectRatio = GR_ASPECT_1x1;

                //TODO Support other color formats
                gt->info.format = GR_TEXFMT_RGB_565;
                quantize_to_rgb565(tb.data, (palette_color_t *)tb.palette, tb.width * tb.height, (unsigned short *)gt->info.data);

                gt->tabletype = -1;
                gt->is_in_tmu = 0;
                gt->tmu_memory_addr = -1;
            }
        }

        unload_bitmap(&tb);
    }

    return is_success;
}

static int is_glide_format(const char* filename)
{
    int is_glide = 0;
    char ext[4], *c;

    c = strrchr(filename, '.');
    if (c != NULL) {
        strcpy(ext, ++c);
        //TODO Compare case insensitive
        if (strcmp("3df", ext) == 0 || strcmp("3DF", ext) == 0) {
            is_glide = 1;
        }
    }

    return is_glide;
}

int load_texture(texture_t* texture, const char* filename) 
{
    int is_success;

    if (is_glide_format(filename)) {
        is_success = load_glide_texture(&texture->glidetexture, filename);
    }
    else {
        is_success = load_bitmap_texture(&texture->glidetexture, filename);
    }
    if (is_success) {
        texture->texturetype = TEXTURE_GLIDE;
        printf("Loaded texture: %s\n", filename);
    }

    return is_success;
}

void unload_texture(texture_t* texture)
{
    unload_glide_texture(&texture->glidetexture);
}
