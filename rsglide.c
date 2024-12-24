
#include <stdlib.h>
#include <stdio.h>
#include <glide.h>

#include "raster.h"
#include "copytlib.h"

static float glidepalette[768];

static void convert_palette(unsigned char* s, float* d, float divisor)
{
    int i;
    unsigned char* ss = s;
    float* dd = d;

    for (i = 0; i < 768; i++) {
        *(dd++) = ( *(ss++) ) / divisor;
    }
}

static float compute_intensity(vector3d_t* normal, vector3d_t* light)
{
    float dot;

    dot = dot_product(normal, light);
    if (dot < 0) {
        dot = 0;
    }
    else if (dot > 1) {
        dot = 1;
    }

    return dot * 255;
}

// Public functions

int create_context(drawcontext_t* dc, unsigned char* palette)
{
    FxBool is_success;
    GrHwConfiguration hwconfig;
    GrScreenResolution_t resolution = GR_RESOLUTION_640x480;

    grGlideInit();

    is_success = grSstQueryHardware(&hwconfig);
    if (is_success == FXTRUE) {
        grSstSelect(0);

        is_success = grSstWinOpen(0, 
            resolution, 
            GR_REFRESH_60Hz, 
            GR_COLORFORMAT_ABGR, 
            GR_ORIGIN_UPPER_LEFT, 
            2, 1);
        if (is_success == FXTRUE) {
            //decal texture
            grTexCombine( GR_TMU0,
                          GR_COMBINE_FUNCTION_LOCAL,
                          GR_COMBINE_FACTOR_NONE,
                          GR_COMBINE_FUNCTION_NONE,
                          GR_COMBINE_FACTOR_NONE,
                          FXFALSE, FXFALSE );

            //flat lit
            /*
            grColorCombine( GR_COMBINE_FUNCTION_SCALE_OTHER,
                            GR_COMBINE_FACTOR_LOCAL,
                            GR_COMBINE_LOCAL_CONSTANT,
                            GR_COMBINE_OTHER_TEXTURE,
                            FXFALSE );
            */

            //rgb lit
            grColorCombine( GR_COMBINE_FUNCTION_SCALE_OTHER,
                            GR_COMBINE_FACTOR_LOCAL,
                            GR_COMBINE_LOCAL_ITERATED,
                            GR_COMBINE_OTHER_TEXTURE,
                            FXFALSE );
            /*
            grAlphaCombine( GR_COMBINE_FUNCTION_LOCAL,
                            GR_COMBINE_FACTOR_NONE,
                            GR_COMBINE_LOCAL_ITERATED,
                            GR_COMBINE_OTHER_TEXTURE,
                            FXFALSE );
            */

            dc->width = 640;
            dc->height = 480;
            grBufferClear(0x00, 0, GR_WDEPTHVALUE_FARTHEST);
            
            convert_palette(palette, glidepalette, 63);
        }
    }

    if (is_success == FXFALSE) {
        printf("Failure to initialize Voodoo graphics system.\n");
    }

    return (is_success == FXTRUE) ? 1 : 0;
}

void destroy_context(drawcontext_t* dc)
{
    grGlideShutdown();
}

void flip_buffer(drawcontext_t *dc)
{
    grBufferSwap(1);
    grSstIdle();

    grBufferClear(0x00, 0, GR_WDEPTHVALUE_FARTHEST);
}

void download_to_tmu(glidetexture_t* gt)
{
    static int download_count;
    static FxU32 last_memory_addr, max_memory_addr;
    FxU32 req_memory;
    TlTexture* tl = (TlTexture *)gt->data;

    // Download texture to TMU onboard memory
    if (!download_count) {
        last_memory_addr = grTexMinAddress(GR_TMU0);
        max_memory_addr = grTexMaxAddress(GR_TMU0);
    }

    req_memory = grTexTextureMemRequired(GR_MIPMAPLEVELMASK_BOTH, &tl->info);

    //TODO Handle out of memory error
    if ((last_memory_addr + req_memory) <= max_memory_addr) {
        grTexDownloadMipMap(GR_TMU0, last_memory_addr, GR_MIPMAPLEVELMASK_BOTH, &tl->info);
        if (tl->tableType != NO_TABLE) {
            grTexDownloadTable(GR_TMU0, tl->tableType, &tl->tableData);
        }
        
        gt->is_in_tmu = 1;
        gt->tmu_memory_addr = (unsigned long)last_memory_addr;
        last_memory_addr += req_memory;
        
        download_count++;
    }
}

void select_texture(glidetexture_t* gt)
{
    static FxU32 last_used_addr;
    TlTexture* tl = (TlTexture *)gt->data;

    if (last_used_addr != (FxU32)(gt->tmu_memory_addr)) {
        if (!gt->is_in_tmu) {
            download_to_tmu(gt);
        }
    
        // Check in case download failed 
        if (gt->is_in_tmu) {
            last_used_addr = (FxU32)(gt->tmu_memory_addr);
            grTexSource(GR_TMU0, last_used_addr, GR_MIPMAPLEVELMASK_BOTH, &tl->info);
        }
    }
}

void draw_object3d(object3d_t* obj, vector3d_t* light, drawcontext_t* dc)
{
    int i;
    float corrx, corry, scalex, scaley;
    facedata_t *f;
    faceorder_t* fo = obj->faceorder;

    float i1, i2, i3;
    GrVertex v1, v2, v3;
    glidetexture_t* gt;

    const vector3d_t lookvector = { -1, 0, 0 };

    corrx = dc->width / 2;
    corry = dc->height / 2;
    scalex = dc->width / 320;
    scaley = dc->height / 240; // aspect ratio!

    sort_faces(obj, &lookvector);

    for (i = 0; i < obj->numvisible; i++) {
        f = fo->face;
        i1 = compute_intensity(&f->v1->rotated_normal, light);
        i2 = compute_intensity(&f->v2->rotated_normal, light);
        i3 = compute_intensity(&f->v3->rotated_normal, light);

        v1.x = f->v1->translated_point.x * scalex + corrx;
        v1.y = f->v1->translated_point.y * scaley + corry;
                                                
        v2.x = f->v2->translated_point.x * scalex + corrx;
        v2.y = f->v2->translated_point.y * scaley + corry;
                                                
        v3.x = f->v3->translated_point.x * scalex + corrx;
        v3.y = f->v3->translated_point.y * scaley + corry;

        gt = &f->mapper.texture->glidetexture;
        select_texture(gt);

        //FIXME
        v1.oow = 1;
        v1.tmuvtx[0].sow = (float)(f->mapper.s1 >> 7) * 255;
        v1.tmuvtx[0].tow = (float)(f->mapper.t1 >> 7) * 255;

        v2.oow = 1;
        v2.tmuvtx[0].sow = (float)(f->mapper.s2 >> 7) * 255;
        v2.tmuvtx[0].tow = (float)(f->mapper.t2 >> 7) * 255;

        v3.oow = 1;
        v3.tmuvtx[0].sow = (float)(f->mapper.s3 >> 7) * 255;
        v3.tmuvtx[0].tow = (float)(f->mapper.t3 >> 7) * 255;

        //FIXME
        v1.r = i1;
        v1.g = i1;
        v1.b = i1;
        
        v2.r = i2;
        v2.g = i2;
        v2.b = i2;
        
        v3.r = i3;
        v3.g = i3;
        v3.b = i3;

        /*
        v1.a = i1;
        v2.a = i2;
        v3.a = i3;
        */

        grDrawTriangle(&v1, &v2, &v3);

        fo++;
    }
}