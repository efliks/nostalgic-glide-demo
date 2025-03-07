#ifndef _ENVMAP_H
#define _ENVMAP_H

typedef enum {
    COLOR_RED,
    COLOR_GREEN,
    COLOR_BLUE
} colortype_t;

typedef struct
{
    float maxcol, scalecol, diffuse, specular;
    colortype_t colortype;
} component_t;

typedef struct
{
    int size;
    float scale, maxcolor;
    component_t red, green, blue;
} config_envmap_t;

void compute_envmap(unsigned char *, config_envmap_t *);

void compute_phong_palette(unsigned char *, config_envmap_t *);

#endif // _ENVMAP_H
