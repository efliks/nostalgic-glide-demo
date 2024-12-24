
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <glide.h>

#include "texture.h"
#include "copytlib.h"

// Copy from 3dfx tlib
static GrTexTable_t texTableType( GrTextureFormat_t format ) 
{
    GrTexTable_t rv = (GrTexTable_t)NO_TABLE;
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

int load_3dffile(Gu3dfInfo* gu, char* filename)
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
    TlTexture* tl;

    if (gt->data) {
        tl = (TlTexture *)gt->data;
        if (tl->info.data) {
            free(tl->info.data);
        }

        free(gt->data);
    }
}

int load_glide_texture(glidetexture_t* gt, char* filename)
{
    int is_good = 0;

    Gu3dfInfo gu;
    TlTexture* tl;

    if (load_3dffile(&gu, filename)) {

        gt->is_in_tmu = 0;
        gt->tmu_memory_addr = -1;
        gt->data = malloc(sizeof(TlTexture));
        if (gt->data != NULL) {
            tl = (TlTexture *)gt->data;

            tl->info.smallLod = gu.header.small_lod;
            tl->info.largeLod = gu.header.large_lod;
            tl->info.aspectRatio = gu.header.aspect_ratio;
            tl->info.format = gu.header.format;
            
            // Think to free this memory !
            tl->info.data = gu.data;

            tl->tableType = texTableType(tl->info.format);

            switch (tl->tableType) {
            case GR_TEXTABLE_NCC0:
            case GR_TEXTABLE_NCC1:
            case GR_TEXTABLE_PALETTE:
                memcpy(&tl->tableData, &(gu.table), sizeof(TlTextureTable));
                break;
            default:
                break;
            }

            is_good = 1;
        }
    }

    if (!is_good) {
        unload_glide_texture(gt);
    }

    return is_good;
}

int load_texture(texture_t* texture, char* filename) 
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
