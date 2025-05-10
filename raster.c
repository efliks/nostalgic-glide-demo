
#include <stdlib.h>

#include "raster.h"
#include "vga13h.h"
#include "ptpoly2.h"

int create_context(drawcontext_t* dc)
{
    dc->soft.framebuffer = (unsigned char *)malloc(64000);
    if (dc->soft.framebuffer != NULL) {
        dc->width = 320;
        dc->height = 200;
        dc->contexttype = CONTEXT_SOFT;
        dc->drawmode = MODE_TEXTURE;
        clear_buffer(dc->soft.framebuffer);
        
        set_mode13h();

        return 1;
    }

    return 0;
}

void destroy_context(drawcontext_t* dc)
{
    unset_mode13h();

    if (dc->soft.framebuffer) {
        free(dc->soft.framebuffer);
    }
}

void set_context_palette(drawcontext_t *dc, unsigned char* palette)
{
    set_palette(palette);
}

void set_draw_mode(drawcontext_t* dc, drawmode_t mode)
{
    dc->drawmode = mode;
}

void flip_buffer(drawcontext_t *dc)
{
    copy_buffer(dc->soft.framebuffer);
    clear_buffer(dc->soft.framebuffer);
}

//TODO Move to common module
static unsigned char compute_intensity(vector3d_t* normal, vector3d_t* light)
{
    float dot;

    dot = dot_product(normal, light);
    if (dot < 0) {
        dot = 0;
    }
    else if (dot > 1) {
        dot = 1;
    }

    //return (unsigned char)(dot * 63);

    // num_levels - 1 = 7
    return (unsigned char)(dot * 62);
}

static void draw_envmapped_triangle(TPolytri* poly, facedata_t* f, drawcontext_t* dc)
{
    int tsi, shifty;
    float ts;

    tsi = f->mapper.texture->bitmap.width;
    if (tsi == 128) {
        shifty = 7;
    }
    else { // (ts == 256)
        shifty = 8;
    }
    ts = (float)(f->mapper.texture->bitmap.width) * 0.5 - 1;

    // Why this works?
    poly->u1 = f->v1->rotated_normal.z * ts + ts;
    poly->v1 = f->v1->rotated_normal.y * ts + ts;
          
    poly->u2 = f->v2->rotated_normal.z * ts + ts;
    poly->v2 = f->v2->rotated_normal.y * ts + ts;
          
    poly->u3 = f->v3->rotated_normal.z * ts + ts;
    poly->v3 = f->v3->rotated_normal.y * ts + ts;
    poly->texture = (char *)f->mapper.texture->bitmap.data;

    drawtpolysubtri(poly, shifty, (char *)dc->soft.framebuffer);
}

static void draw_gouraud_triangle(TPolytri* poly, facedata_t* f, vector3d_t* lightvector, drawcontext_t* dc)
{
    unsigned char c1, c2, c3;

    c1 = compute_intensity(&f->v1->rotated_normal, lightvector);
    c2 = compute_intensity(&f->v2->rotated_normal, lightvector);
    c3 = compute_intensity(&f->v3->rotated_normal, lightvector);
    
    //TODO
}

static void draw_textured_triangle(TPolytri* poly, facedata_t* f, drawcontext_t* dc)
{
    int ts, tt, shifty;

    //TODO Optimize
    ts = f->mapper.texture->bitmap.width;
    tt = f->mapper.texture->bitmap.height;

    poly->u1 = (float)(f->mapper.s1 * ts);
    poly->v1 = (float)(f->mapper.t1 * tt);
    poly->u2 = (float)(f->mapper.s2 * ts);
    poly->v2 = (float)(f->mapper.t2 * tt);
    poly->u3 = (float)(f->mapper.s3 * ts);
    poly->v3 = (float)(f->mapper.t3 * tt);
    poly->texture = (char *)f->mapper.texture->bitmap.data;

    if (ts == 128) {
        shifty = 7;
    }
    else { // (ts == 256)
        shifty = 8;
    }

    drawtpolyperspdivsubtri(poly, shifty, (char *)dc->soft.framebuffer);
}

void draw_object3d(object3d_t* obj, vector3d_t* lightvector, drawcontext_t* dc)
{
    int i;
    TPolytri poly;
    float corrx, corry;
    facedata_t *f;
    faceorder_t* fo = obj->faceorder;

    const vector3d_t lookvector = { -1, 0, 0 };

    sort_faces(obj, &lookvector);

    corrx = dc->width / 2;
    corry = dc->height / 2;

    for (i = 0; i < obj->numvisible; i++) {
        f = fo->face;
        poly.x1 = f->v1->translated_point.x + corrx;
        poly.y1 = f->v1->translated_point.y + corry;
        poly.z1 = 1. / f->v1->oow;
          
        poly.x2 = f->v2->translated_point.x + corrx;
        poly.y2 = f->v2->translated_point.y + corry;
        poly.z2 = 1. / f->v2->oow;
          
        poly.x3 = f->v3->translated_point.x + corrx;
        poly.y3 = f->v3->translated_point.y + corry;
        poly.z3 = 1. / f->v3->oow;

        switch (dc->drawmode) {
        case MODE_TEXTURE:
            draw_textured_triangle(&poly, f, dc);
            break;
        case MODE_ENVMAP:
            draw_envmapped_triangle(&poly, f, dc);
            break;
        case MODE_GOURAUD:
            draw_gouraud_triangle(&poly, f, lightvector, dc);
            break;
        default:
            break;
        }

        fo++;
    }
}
