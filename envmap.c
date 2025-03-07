
#include <math.h>
#include <stdlib.h>

#include "envmap.h"

typedef struct
{
    unsigned char r, g, b;
} palette_color_t;

static void compute_component(palette_color_t* pal, float diff, float spec, float maxcol, float scalecol, colortype_t what_color)
{
    int i;

    const float c_pi = 3.14159265358979323846;
    const float spec_exp = 12;
    const int color_count = 63;
    const float delta = (c_pi / 2) / color_count;

    float angle = c_pi / 2;
    float lum;
    unsigned char color;

    //debug!!
    //printf("diffuse = %f, specular = %f\n", diff, spec);

    for (i = 0; i <= color_count; i++) {
        lum = diff * cos(angle) + spec * pow(cos(angle), spec_exp);
        if (lum > maxcol) {
            lum = maxcol;
        }
        lum *= scalecol;
        color = (unsigned char)lum;
        angle -= delta;

        switch (what_color) {
        case COLOR_RED:
            pal->r = color;
            break;
        case COLOR_GREEN:
            pal->g = color;
            break;
        case COLOR_BLUE:
            pal->b = color;
            break;
        default:
            break;
        }
       
        pal++;
    }
}

void compute_phong_palette(unsigned char* palette, config_envmap_t* cfg)
{
    int i;

    /*
    const colortype_t types[] = { COLOR_RED, COLOR_GREEN, COLOR_BLUE };
    const float maxcolor[] = { 31, 63, 31 };
    const float scalecolor[] = { 2, 1, 2 };
    */
    component_t components[3];
    component_t* c = components;
    components[0] = cfg->red;
    components[1] = cfg->green;
    components[2] = cfg->blue;

    for (i = 0; i < 3; i++) {
        //compute_component((palette_color_t *)palette, (float)(rand() % 20), (float)(rand() % 50), maxcolor[i], scalecolor[i], types[i]);
        compute_component((palette_color_t *)palette, c->diffuse, c->specular, c->maxcol, c->scalecol, c->colortype);
        c++;
    }
}

void compute_envmap(unsigned char *envmap, config_envmap_t* cfg)
{
    int i, j;
    float x, y, color, w;
    unsigned char *ep;

    w = 2 / (float)cfg->size;
    ep = envmap;

    for (i = 0; i < cfg->size; i++) {
        for (j = 0; j < cfg->size; j++) {
            x = (float)i * w - 1;
            y = (float)j * w - 1;
            color = (1 - sqrt(pow(x, 2) + pow(y, 2))) * cfg->maxcolor * cfg->scale;
            if (color < 0) {
                color = 0;
            }
            else if (color > cfg->maxcolor) {
                color = cfg->maxcolor;
            }
            *ep = (unsigned char)color;
            ep++;
        }
    }
}
