
#include <math.h>

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

void compute_phong_palette(unsigned char* palette)
{
    int i;

    /*
    const colortype_t types[] = { COLOR_RED, COLOR_GREEN, COLOR_BLUE };
    float maxcolor[] = { 31, 63, 31 };
    float scalecolor[] = { 2, 1, 2 };
    float diffuse[] = { 4, 16, 16 };
    float specular[] = { 27, 20, 48 };
    */
    const component_t components[] = { { 31, 2, 4, 27, COLOR_RED }, { 63, 1, 16, 20, COLOR_GREEN }, { 31, 2, 16, 48, COLOR_BLUE } };
    const component_t* c = components;

    for (i = 0; i < 3; i++) {
        //compute_component((palette_color_t *)palette, (float)(rand() % 20), (float)(rand() % 50), maxcolor[i], scalecolor[i], types[i]);
        compute_component((palette_color_t *)palette, c->diffuse, c->specular, c->maxcol, c->scalecol, c->colortype);
        c++;
    }
}

void compute_envmap(unsigned char* envmap, int size, float maxcolor, float scale)
{
    int i, j;
    float x, y, color, w;
    unsigned char *ep;

    w = 2 / (float)size;
    ep = envmap;

    for (i = 0; i < size; i++) {
        for (j = 0; j < size; j++) {
            x = (float)i * w - 1;
            y = (float)j * w - 1;
            color = (1 - sqrt(pow(x, 2) + pow(y, 2))) * maxcolor * scale;
            if (color < 0) {
                color = 0;
            }
            else if (color > maxcolor) {
                color = maxcolor;
            }
            *ep = (unsigned char)color;
            ep++;
        }
    }
}
