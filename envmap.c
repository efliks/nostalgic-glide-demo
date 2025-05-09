
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

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

static void compute_phong_palette(unsigned char* palette, config_envmap_t* cfg)
{
    int i;

    component_t components[3];
    component_t* c = components;
    components[0] = cfg->red;
    components[1] = cfg->green;
    components[2] = cfg->blue;

    for (i = 0; i < 3; i++) {
        compute_component((palette_color_t *)palette, c->diffuse, c->specular, c->maxcol, c->scalecol, c->colortype);
        c++;
    }
}

static void randomize_component(component_t* c, colortype_t colortype)
{
    switch (colortype) {
    case COLOR_RED:
        c->maxcol = 31;
        c->scalecol = 2;
        break;
    case COLOR_GREEN:
        c->maxcol = 63;
        c->scalecol = 1;
        break;
    case COLOR_BLUE:
        c->maxcol = 31;
        c->scalecol = 2;
        break;
    default:
        // should never happen
        break;
    }

    c->diffuse = (float)(rand() % 20);
    c->specular = (float)(rand() % 50);
        
    c->colortype = colortype;
}

static void compute_random_palette(unsigned char* palette, config_envmap_t* cfg)
{
    int i;
    const colortype_t types[] = { COLOR_RED, COLOR_GREEN, COLOR_BLUE };
    component_t* components[3];
    component_t** cp = components;
    components[0] = &cfg->red;
    components[1] = &cfg->green;
    components[2] = &cfg->blue;

    for (i = 0; i < 3; i++) {
        randomize_component(*cp, types[i]);
        cp++;
    }

    compute_phong_palette(palette, cfg);
}

static void compute_envmap(unsigned char* envmap, int size, float scale, float maxcolor)
{
    int i, j;
    float x, y, color;

    float w = 2 / (float)size;

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

            *envmap = (unsigned char)color;
            envmap++;
        }
    }
}

void recompute_envmap(bitmap_t* bitmap, config_envmap_t* c)
{
    compute_random_palette(bitmap->palette, c);
}

int create_envmap(bitmap_t* bitmap, config_envmap_t* c, int is_auto)
{
    char text[54];
    int is_success = 0;
    if (create_empty_bitmap(bitmap, c->size)) {
        compute_envmap(bitmap->data, c->size, c->scale, c->maxcolor);
        if (is_auto) {
            compute_random_palette(bitmap->palette, c);
        }
        else {
            compute_phong_palette(bitmap->palette, c);
        }

        // ignore errors
        config_to_text(c, text);
        store_bitmap_with_text(bitmap, "myenvmap.pcx", text);

        is_success = 1;
    }

    return is_success;
}

void destroy_envmap(bitmap_t* bitmap)
{
    unload_bitmap(bitmap);
}

void config_to_text(config_envmap_t* cfg, char* text)
{
    int i;
    char tmp[20];

    component_t components[3];
    component_t* c = components;
    components[0] = cfg->red;
    components[1] = cfg->green;
    components[2] = cfg->blue;

    strcpy(text, "envmap:");

    for (i = 0; i < 3; i++) {
        sprintf(tmp, "%d,%d", (int)c->diffuse, (int)c->specular);
        strcat(text, tmp);
        if (i != 2) {
            strcat(text, "@");
        }

        c++;
    }
}
