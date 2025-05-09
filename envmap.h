#ifndef _ENVMAP_H
#define _ENVMAP_H

#include "bitmap.h"

typedef enum {
    COLOR_RED,
    COLOR_GREEN,
    COLOR_BLUE
} colortype_t;

// structures

typedef struct
{
    float maxcol;
    float scalecol;
    float diffuse;
    float specular;
    colortype_t colortype;
} component_t;

typedef struct
{
    int is_auto;
    int size;
    float scale;
    float maxcolor;
    component_t red;
    component_t green;
    component_t blue;
} config_envmap_t;


// functions

int create_envmap(bitmap_t *, config_envmap_t *, int);
void recompute_envmap(bitmap_t *, config_envmap_t *);

void destroy_envmap(bitmap_t *);

void config_to_text(config_envmap_t *, char *);

#endif // _ENVMAP_H
