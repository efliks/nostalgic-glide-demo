
#include <stdlib.h>

#include "raster.h"
#include "vga13h.h"
#include "bumptri.h"

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

static void draw_envmapped_triangle(vertex_t* v1, vertex_t* v2, vertex_t* v3, facedata_t* f, drawcontext_t* dc)
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
    v1->tx = (int)(f->v1->rotated_normal.z * ts + ts);
    v1->ty = (int)(f->v1->rotated_normal.y * ts + ts);

    v2->tx = (int)(f->v2->rotated_normal.z * ts + ts);
    v2->ty = (int)(f->v2->rotated_normal.y * ts + ts);

    v3->tx = (int)(f->v3->rotated_normal.z * ts + ts);
    v3->ty = (int)(f->v3->rotated_normal.y * ts + ts);

    textured_triangle(v1, v2, v3, shifty, f->mapper.texture->bitmap.data, dc->soft.framebuffer);
}

static void draw_gouraud_triangle(vertex_t* v1, vertex_t* v2, vertex_t* v3, facedata_t* f, vector3d_t* lightvector, drawcontext_t* dc)
{
    v1->c = compute_intensity(&f->v1->rotated_normal, lightvector);
    v2->c = compute_intensity(&f->v2->rotated_normal, lightvector);
    v3->c = compute_intensity(&f->v3->rotated_normal, lightvector);
    
    gouraud_triangle(v1, v2, v3, dc->soft.framebuffer);
}

static void draw_textured_triangle(vertex_t* v1, vertex_t* v2, vertex_t* v3, facedata_t* f, drawcontext_t* dc)
{
    int ts, tt, shifty;

    //TODO Optimize
    ts = f->mapper.texture->bitmap.width;
    tt = f->mapper.texture->bitmap.height;
    v1->tx = f->mapper.s1 * ts;
    v1->ty = f->mapper.t1 * tt;
    v2->tx = f->mapper.s2 * ts;
    v2->ty = f->mapper.t2 * tt;
    v3->tx = f->mapper.s3 * ts;
    v3->ty = f->mapper.t3 * tt;

    if (ts == 128) {
        shifty = 7;
    }
    else { // (ts == 256)
        shifty = 8;
    }

    textured_triangle(v1, v2, v3, shifty, f->mapper.texture->bitmap.data, dc->soft.framebuffer);
}

void draw_object3d(object3d_t* obj, vector3d_t* lightvector, drawcontext_t* dc)
{
    int i;
    vertex_t v1, v2, v3;
    float corrx, corry;
    facedata_t *f;
    faceorder_t* fo = obj->faceorder;

    const vector3d_t lookvector = { -1, 0, 0 };

    sort_faces(obj, &lookvector);

    corrx = dc->width / 2;
    corry = dc->height / 2;

    for (i = 0; i < obj->numvisible; i++) {
        f = fo->face;
        v1.x = (int)(f->v1->translated_point.x + corrx);
        v1.y = (int)(f->v1->translated_point.y + corry);

        v2.x = (int)(f->v2->translated_point.x + corrx);
        v2.y = (int)(f->v2->translated_point.y + corry);

        v3.x = (int)(f->v3->translated_point.x + corrx);
        v3.y = (int)(f->v3->translated_point.y + corry);

        switch (dc->drawmode) {
        case MODE_TEXTURE:
            draw_textured_triangle(&v1, &v2, &v3, f, dc);
            break;
        case MODE_ENVMAP:
            draw_envmapped_triangle(&v1, &v2, &v3, f, dc);
            break;
        case MODE_GOURAUD:
            draw_gouraud_triangle(&v1, &v2, &v3, f, lightvector, dc);
            break;
        default:
            break;
        }

        fo++;
    }
}
