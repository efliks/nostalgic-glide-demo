
#include "raster.h"
#include "low.h"

void flip_buffer(drawcontext_t *dc)
{
    copy_buffer(dc->framebuffer);
    clear_buffer(dc->framebuffer);
}

void draw_object3d(object3d_t* obj, vector3d_t* light, drawcontext_t* dc)
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
