
#include <stdlib.h>

#include "raster.h"
#include "low.h"
#include "bumptri.h"

int create_context(drawcontext_t* dc, unsigned char* palette)
{
    dc->framebuffer = (unsigned char *)malloc(64000);
    if (dc->framebuffer != NULL) {
        dc->width = 320;
        dc->height = 200;
        clear_buffer(dc->framebuffer);
        
        set_mode13h();
        set_palette(palette);

        return 1;
    }

    return 0;
}

void destroy_context(drawcontext_t* dc)
{
    unset_mode13h();

    if (dc->framebuffer) {
        free(dc->framebuffer);
    }
}

void flip_buffer(drawcontext_t *dc)
{
    copy_buffer(dc->framebuffer);
    clear_buffer(dc->framebuffer);
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

// TODO
int is_envmapped(object3d_t* obj)
{
    int i;
    static is_envmap = -1;
    facedata_t *f = obj->faces;
    texture_t* texture = NULL;

    if (is_envmap == -1) {
        is_envmap = 1;
        for (i = 0; i < obj->numfaces; i++, f++) {
            if (texture == NULL) {
                texture = f->mapper.texture;
            }
            else if (texture != f->mapper.texture) {
                is_envmap = 0;
                break;
            }
        }
    }

    return is_envmap;
}

void draw_envmapped_triangle(int x1, int y1, int x2, int y2, int x3, int y3, facedata_t* f, drawcontext_t* dc)
{
    int tx1, ty1, tx2, ty2, tx3, ty3;

    // Why this works?
    tx1 = (int)(f->v1->rotated_normal.z * 63 + 63);
    ty1 = (int)(f->v1->rotated_normal.y * 63 + 63);

    tx2 = (int)(f->v2->rotated_normal.z * 63 + 63);
    ty2 = (int)(f->v2->rotated_normal.y * 63 + 63);

    tx3 = (int)(f->v3->rotated_normal.z * 63 + 63);
    ty3 = (int)(f->v3->rotated_normal.y * 63 + 63);

    textured_triangle(x1, y1, x2, y2, x3, y3, tx1, ty1, tx2, ty2, tx3, ty3, f->mapper.texture->bitmap.data, dc->framebuffer);
}

void draw_object3d(object3d_t* obj, vector3d_t* lightvector, drawcontext_t* dc)
{
    int i, x1, y1, x2, y2, x3, y3, is_envmap;
    float corrx, corry;
    unsigned char cola, colb, colc;
    facedata_t *f;
    faceorder_t* fo = obj->faceorder;

    const vector3d_t lookvector = { -1, 0, 0 };

    sort_faces(obj, &lookvector);
    is_envmap = is_envmapped(obj);

    corrx = dc->width / 2;
    corry = dc->height / 2;

    for (i = 0; i < obj->numvisible; i++) {
        f = fo->face;
        x1 = (int)(f->v1->translated_point.x + corrx);
        y1 = (int)(f->v1->translated_point.y + corry);

        x2 = (int)(f->v2->translated_point.x + corrx);
        y2 = (int)(f->v2->translated_point.y + corry);

        x3 = (int)(f->v3->translated_point.x + corrx);
        y3 = (int)(f->v3->translated_point.y + corry);

        if (f->mapper.texture != NULL) {
            if (is_envmap) {
                draw_envmapped_triangle(x1, y1, x2, y2, x3, y3, f, dc);
            }
            else {
                textured_triangle(x1, y1, x2, y2, x3, y3, f->mapper.s1, f->mapper.t1, f->mapper.s2, f->mapper.t2, f->mapper.s3, f->mapper.t3, f->mapper.texture->bitmap.data, dc->framebuffer);
            }
        }
        else {
            cola = compute_intensity(&f->v1->rotated_normal, lightvector);
            colb = compute_intensity(&f->v2->rotated_normal, lightvector);
            colc = compute_intensity(&f->v3->rotated_normal, lightvector);
        
            gouraud_triangle(x1, y1, x2, y2, x3, y3, cola, colb, colc, dc->framebuffer);
        }

        fo++;
    }
}

void draw_points(object3d_t* obj, vector3d_t* light, drawcontext_t* dc)
{
    int i, x, y;
    float corrx, corry;
    vertexdata_t* v = obj->vertices;

    corrx = dc->width / 2;
    corry = dc->height / 2;

    for (i = 0; i < obj->numpoints; i++) {
        x = (int)(v->translated_point.x + corrx);
        y = (int)(v->translated_point.y + corry);

        dc->framebuffer[x + y * 320] = 31;
        v++;
    }

    /*
    faceorder_t* fo = obj->faceorder;

    for (i = 0; i < obj->numvisible; i++) {
        fo++;
    }
    */
}
