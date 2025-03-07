#ifndef _ENVMAP_H
#define _ENVMAP_H

typedef enum
{
    COLOR_RED,
    COLOR_GREEN,
    COLOR_BLUE
} colortype_t;

typedef struct
{
    float maxcol;
    float scalecol;
    float diffuse;
    float specular;
    colortype_t colortype;
} component_t;

void compute_envmap(unsigned char *, int, float, float);
void compute_phong_palette(unsigned char *);

#endif // _ENVMAP_H
