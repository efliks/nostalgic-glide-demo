#ifndef _TEXTURE_H
#define _TEXTURE_H

#include <glide.h>

#include "bitmap.h"

typedef enum { 
    TEXTURE_SOFT, 
    TEXTURE_GLIDE 
} texturetype_t;

// structures

typedef struct
{
    int is_in_tmu;
    unsigned long tmu_memory_addr;

    GrTexInfo info;
    GrTexTable_t tabletype;
    GuTexTable table;
} glidetexture_t;

typedef struct
{
    union {
        bitmap_t bitmap;
        glidetexture_t glidetexture;
    };

    texturetype_t texturetype;
} texture_t;

// functions

int load_texture(texture_t *, const char *);
void unload_texture(texture_t *);

#endif // _TEXTURE_H
