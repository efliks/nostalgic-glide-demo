
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <glide.h>

#include "texture.h"

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

int load_3dffile(Gu3dfInfo* gu, const char* filename)
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

void unload_glide_texture(glidetexture_t* gt)
{
    if (gt->info.data != NULL) {
        free(gt->info.data);
    }
}

int load_glide_texture(glidetexture_t* gt, const char* filename)
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

int load_texture(texture_t* texture, const char* filename) 
{
    int is_success;

    is_success = load_glide_texture(&texture->glidetexture, filename);
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

void copy_texture(texture_t* dest, texture_t* src)
{
    dest->glidetexture.is_in_tmu = src->glidetexture.is_in_tmu;
    dest->glidetexture.tmu_memory_addr = src->glidetexture.tmu_memory_addr;

    dest->glidetexture.info.smallLod = src->glidetexture.info.smallLod;
    dest->glidetexture.info.largeLod = src->glidetexture.info.largeLod;
    dest->glidetexture.info.aspectRatio = src->glidetexture.info.aspectRatio;
    dest->glidetexture.info.format = src->glidetexture.info.format;
    // shallow copy is good enough
    dest->glidetexture.info.data = src->glidetexture.info.data;

    dest->glidetexture.tabletype = src->glidetexture.tabletype;
    memcpy(&dest->glidetexture.table, &src->glidetexture.table, sizeof(GuTexTable));

    dest->texturetype = src->texturetype;
}
