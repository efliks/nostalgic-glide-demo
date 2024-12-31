
#include <stdlib.h>
#include <stdio.h>
#include <glide.h>

#include "raster.h"

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

int create_context(drawcontext_t* dc)
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
            dc->width = 640;
            dc->height = 480;
            dc->contexttype = CONTEXT_GLIDE;
            grBufferClear(0x00, 0, GR_WDEPTHVALUE_FARTHEST);

            dc->glide.last_memory_addr = grTexMinAddress(GR_TMU0);
            dc->glide.max_memory_addr = grTexMaxAddress(GR_TMU0);
            dc->glide.last_used_addr = 0;
            dc->glide._download_count = 0;

            //decal texture
            grTexCombine( GR_TMU0,
                          GR_COMBINE_FUNCTION_LOCAL,
                          GR_COMBINE_FACTOR_NONE,
                          GR_COMBINE_FUNCTION_NONE,
                          GR_COMBINE_FACTOR_NONE,
                          FXFALSE, FXFALSE );
        
            set_draw_mode(dc, MODE_TEXTURE);    
        }
    }

    if (is_success == FXFALSE) {
        printf("Failure to initialize Voodoo graphics system.\n");
    }

    return (is_success == FXTRUE) ? 1 : 0;
}

void set_context_palette(drawcontext_t *dc, unsigned char* palette)
{
    // Do nothing
}

void set_draw_mode(drawcontext_t* dc, drawmode_t mode)
{
    dc->drawmode = mode;

    switch (dc->drawmode) {
    case MODE_ENVMAP:
        // flat lit
        grColorCombine(GR_COMBINE_FUNCTION_SCALE_OTHER, 
            GR_COMBINE_FACTOR_LOCAL, 
            GR_COMBINE_LOCAL_CONSTANT, 
            GR_COMBINE_OTHER_TEXTURE, FXFALSE);
        break;
    case MODE_GOURAUD:
        grColorCombine(GR_COMBINE_FUNCTION_LOCAL,
            GR_COMBINE_FACTOR_NONE,
            GR_COMBINE_LOCAL_ITERATED,
            GR_COMBINE_OTHER_NONE,
            FXFALSE);
        break;
    case MODE_TEXTURE:
        // rgb lit
        grColorCombine(GR_COMBINE_FUNCTION_SCALE_OTHER, 
            GR_COMBINE_FACTOR_LOCAL, 
            GR_COMBINE_LOCAL_ITERATED, 
            GR_COMBINE_OTHER_TEXTURE, FXFALSE);
        break;
    }
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

void download_to_tmu(glidetexture_t* gt, drawcontext_t* dc)
{
    FxU32 req_memory;

    // Download texture to TMU onboard memory
    req_memory = grTexTextureMemRequired(GR_MIPMAPLEVELMASK_BOTH, &gt->info);

    //TODO Handle out of memory error
    if ((dc->glide.last_memory_addr + req_memory) <= dc->glide.max_memory_addr) {
        grTexDownloadMipMap(GR_TMU0, dc->glide.last_memory_addr, GR_MIPMAPLEVELMASK_BOTH, &gt->info);
        if (gt->tabletype != -1) {
            grTexDownloadTable(GR_TMU0, gt->tabletype, &gt->table);
        }
        
        gt->is_in_tmu = 1;
        gt->tmu_memory_addr = (unsigned long)dc->glide.last_memory_addr;
        dc->glide.last_memory_addr += req_memory;

        dc->glide._download_count++;
    }
}

void select_texture(glidetexture_t* gt, drawcontext_t* dc)
{
    if (dc->glide.last_used_addr != (FxU32)(gt->tmu_memory_addr)) {
        if (!gt->is_in_tmu) {
            download_to_tmu(gt, dc);
        }
    
        // Check in case download failed 
        if (gt->is_in_tmu) {
            dc->glide.last_used_addr = (FxU32)(gt->tmu_memory_addr);
            grTexSource(GR_TMU0, dc->glide.last_used_addr, GR_MIPMAPLEVELMASK_BOTH, &gt->info);
        }
    }
}

void configure_envmapped_triangle(GrVertex* v1, GrVertex* v2, GrVertex* v3, facedata_t* f)
{
    v1->tmuvtx[0].sow = (f->v1->rotated_normal.z * 127 + 127) * f->v1->oow;
    v1->tmuvtx[0].tow = (f->v1->rotated_normal.y * 127 + 127) * f->v1->oow;

    v2->tmuvtx[0].sow = (f->v2->rotated_normal.z * 127 + 127) * f->v2->oow;
    v2->tmuvtx[0].tow = (f->v2->rotated_normal.y * 127 + 127) * f->v2->oow;

    v3->tmuvtx[0].sow = (f->v3->rotated_normal.z * 127 + 127) * f->v3->oow;
    v3->tmuvtx[0].tow = (f->v3->rotated_normal.y * 127 + 127) * f->v3->oow;
}

void configure_textured_triangle(GrVertex* v1, GrVertex* v2, GrVertex* v3, facedata_t* f)
{
    //FIXME
    v1->tmuvtx[0].sow = (float)(f->mapper.s1 >> 7) * 255 * f->v1->oow;
    v1->tmuvtx[0].tow = (float)(f->mapper.t1 >> 7) * 255 * f->v1->oow;

    v2->tmuvtx[0].sow = (float)(f->mapper.s2 >> 7) * 255 * f->v2->oow;
    v2->tmuvtx[0].tow = (float)(f->mapper.t2 >> 7) * 255 * f->v2->oow;

    v3->tmuvtx[0].sow = (float)(f->mapper.s3 >> 7) * 255 * f->v3->oow;
    v3->tmuvtx[0].tow = (float)(f->mapper.t3 >> 7) * 255 * f->v3->oow;
}

void configure_gouraud_triangle(GrVertex* v1, GrVertex* v2, GrVertex* v3, facedata_t* f, vector3d_t* light)
{
    float i1, i2, i3;

    i1 = compute_intensity(&f->v1->rotated_normal, light);
    v1->r = i1;
    v1->g = i1;
    v1->b = i1;
    
    i2 = compute_intensity(&f->v2->rotated_normal, light);
    v2->r = i2;
    v2->g = i2;
    v2->b = i2;
    
    i3 = compute_intensity(&f->v3->rotated_normal, light);
    v3->r = i3;
    v3->g = i3;
    v3->b = i3;
}

void draw_object3d(object3d_t* obj, vector3d_t* light, drawcontext_t* dc)
{
    int i;
    float corrx, corry, scalex, scaley;
    facedata_t *f;
    faceorder_t* fo = obj->faceorder;

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
        v1.x = f->v1->translated_point.x * scalex + corrx;
        v1.y = f->v1->translated_point.y * scaley + corry;
                                                
        v2.x = f->v2->translated_point.x * scalex + corrx;
        v2.y = f->v2->translated_point.y * scaley + corry;
                                                
        v3.x = f->v3->translated_point.x * scalex + corrx;
        v3.y = f->v3->translated_point.y * scaley + corry;

        gt = &f->mapper.texture->glidetexture;
    
        v1.oow = f->v1->oow;
        v2.oow = f->v2->oow;
        v3.oow = f->v3->oow;

        switch (dc->drawmode) {
        case MODE_TEXTURE:
            select_texture(gt, dc);
            configure_textured_triangle(&v1, &v2, &v3, f);
            configure_gouraud_triangle(&v1, &v2, &v3, f, light);
            break;
        case MODE_ENVMAP:
            select_texture(gt, dc);
            configure_envmapped_triangle(&v1, &v2, &v3, f);
            break;
        case MODE_GOURAUD:
            configure_gouraud_triangle(&v1, &v2, &v3, f, light);
            break;
        default:
            break;
        }

        grDrawTriangle(&v1, &v2, &v3);

        fo++;
    }
}
