#ifndef _RASTER_H
#define _RASTER_H

#include "bumpobj.h"

#include <glide.h>

typedef enum { 
    CONTEXT_SOFT, 
    CONTEXT_GLIDE 
} contexttype_t;

typedef enum { 
    MODE_GOURAUD, 
    MODE_ENVMAP, 
    MODE_TEXTURE 
} drawmode_t;

// structures

typedef struct
{
    int _download_count;
    FxU32 last_memory_addr, max_memory_addr;
    FxU32 last_used_addr;
} glidecontext_t;

typedef struct
{
    unsigned char* framebuffer;
} softcontext_t;

typedef struct
{
    union {
        softcontext_t soft;
        glidecontext_t glide;
    };
    contexttype_t contexttype;
    drawmode_t drawmode;

    float width, height;
} drawcontext_t;


// functions

void draw_object3d(object3d_t *, vector3d_t *, drawcontext_t *);

void flip_buffer(drawcontext_t *);

int create_context(drawcontext_t *);
void destroy_context(drawcontext_t *);
void set_context_palette(drawcontext_t *, unsigned char *);
void set_draw_mode(drawcontext_t *, drawmode_t);

void reset_context(drawcontext_t *);

#endif  // _RASTER_H
