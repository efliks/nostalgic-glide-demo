
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "low.h"
#include "clock.h"

#include "bumpobj.h"
#include "raster.h"
#include "texture.h"

const char* texturefiles[20] = { "assets/brick.pcx", "assets/browntil.pcx", "assets/floor.pcx", "assets/brick.pcx", "assets/browntil.pcx", "assets/floor.pcx" };

int get_next_mode(int drawing_mode)
{
    // next mode not found -> should exit
    return -1;
}

void main_loop(object3d_t* obj, vector3d_t* light_source, drawcontext_t* drawcontext)
{
    int drawing_mode = 0;

    while (drawing_mode != -1) {
        timer_start(5);

        update_object3d(obj);
        draw_object3d(obj, light_source, drawcontext);

        flip_buffer(drawcontext);

        timer_wait();

        if (is_key_pressed()) {
            get_key_code();
            drawing_mode = get_next_mode(drawing_mode);
        }
    }
}

/*
void unload_textures(TEXTURE* textures)
{
    if (textures[0].data) {
        unload_texture(&textures[0]);
        if (textures[1].data) {
            unload_texture(&textures[1]);
            if (textures[2].data) {
                unload_texture(&textures[2]);
            }
        }
    }
}

int load_textures(texture_t* textures)
{
    memset(textures, 0, 3 * sizeof(texture_t));

    if (load_texture(&textures[0], "assets/brick.pcx") == 1) {
        if (load_texture(&textures[1], "assets/floor.pcx") == 1) {
            if (load_texture(&textures[2], "assets/browntil.pcx") == 1) {
                return 1;
            }
        }
    }

    unload_textures(textures);
    return 0;
}
*/

void write_palette(unsigned char* palette, char* filename)
{
    int i;
    FILE* fp = fopen(filename, "w");

    if (fp) {
        for (i = 0; i < 256; i++) {
            fprintf(fp, "%d, %d, %d\n", palette[i * 3], palette[i * 3 + 1], palette[i * 3 + 2]);
        }

        fclose(fp);
    }
}

unsigned char compute_component(unsigned char maxcolor, int num_levels, 
    int curr_level)
{
    unsigned long tmpcolor = (unsigned long)maxcolor;

    return (unsigned char)((((tmpcolor << 8) / (num_levels - 1)) * curr_level) >> 8); 

    /*
    float tmpcolor = (float)(maxcolor) / num_levels * curr_level;

    if (tmpcolor < 0.f) {
        return 0;
    }
    else if (tmpcolor > 63.f) {
        return 63;
    }

    return (unsigned char)(tmpcolor);
    */
}

void compute_palette_levels(unsigned char* inpal, unsigned char* outpal, 
    int num_colors)
{
    int i, j, k, num_levels;
    unsigned char *inp, *outp;

    num_levels = 256 / num_colors;

    for (i = 0; i < num_colors; i++) {
        for (j = 0; j < num_levels; j++) {
            inp = inpal + 3 * i;
            outp = outpal + 3 * (i + j * num_colors);

            for (k = 0; k < 3; k++) {
                *outp = compute_component(*inp, num_levels, j);
                outp++;
                inp++;
            }
        }
    }

    write_palette(outpal, "genpal.txt");
}

void do_bump_mapping(unsigned char* framebuffer)
{
    int is_success = 0;

    drawcontext_t dc;
    object3d_t objcube;
    vector3d_t light = { -1, 0, 0 };

    dc.width = 320;
    dc.height = 200;
    dc.framebuffer = framebuffer;

    is_success = create_cube(&objcube, texturefiles, 6);
    if (is_success) {
        reset_and_scale_object3d(&objcube, 100.f);
        objcube.adx = 1;
        objcube.ady = 1;
        objcube.adz = 1;
        set_palette(objcube.textures[0].palette);
   
        main_loop(&objcube, &light, &dc);
    
        unload_object3d(&objcube);
    }

    if (!is_success) {
        //TODO 
    }
}
