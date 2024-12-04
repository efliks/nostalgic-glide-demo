
#include "raster.h"
#include "low.h"
#include "bumptri.h"

void flip_buffer(drawcontext_t *dc)
{
    copy_buffer(dc->framebuffer);
    clear_buffer(dc->framebuffer);
}

void draw_object3d(object3d_t* obj, vector3d_t* light, drawcontext_t* dc)
{
    int i, x1, y1, x2, y2, x3, y3;
    float corrx, corry;
    facedata_t *f;
    faceorder_t* fo = obj->faceorder;

    const vector3d_t lookvector = { -1, 0, 0 };

    sort_faces(obj, &lookvector);

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

        //flat_triangle(x1, y1, x2, y2, x3, y3, (unsigned char)(i + 1), dc->framebuffer);
        textured_triangle(x1, y1, x2, y2, x3, y3, f->mapper.s1, f->mapper.t1, f->mapper.s2, f->mapper.t2, f->mapper.s3, f->mapper.t3, f->mapper.texture->data, dc->framebuffer);

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
