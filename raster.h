#ifndef _RASTER_H
#define _RASTER_H

#include "bumpobj.h"

typedef struct
{
    float width, height;
    unsigned char* framebuffer;
} drawcontext_t;

void draw_object3d(object3d_t *, vector3d_t *, drawcontext_t *);

void flip_buffer(drawcontext_t *);

int create_context(drawcontext_t *, unsigned char *);
void destroy_context(drawcontext_t *);

#endif  // _RASTER_H
