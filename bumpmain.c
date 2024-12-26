
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "low.h"
#include "clock.h"

#include "bumpobj.h"
#include "raster.h"
#include "texture.h"
#include "texman.h"

//const char* texturefiles[20] = { "assets/work/col32/metlplt.pcx", "assets/work/col32/trak_g.pcx" , "assets/work/col32/02camino.pcx", "assets/work/col32/floor_d.pcx", "assets/work/col32/floor_f.pcx", "assets/work/col32/floor_g.pcx" };

const char* texturefiles[20] = { "assets/glide/metlplt.3df", "assets/glide/trak_g.3df" , "assets/glide/02camino.3df", "assets/glide/floor_d.3df", "assets/glide/floor_f.3df", "assets/glide/floor_g.3df" };

int get_next_mode(int drawing_mode)
{
    // next mode not found -> should exit
    return -1;
}

void main_loop(object3d_t* obj, vector3d_t* light_source, drawcontext_t* drawcontext)
{
    int drawing_mode = 0;

    while (drawing_mode != -1) {
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
                *(outp++) = compute_component(*(inp++), num_levels, j);
            }
        }
    }

    write_palette(outpal, "genpal.txt");
}

// debug!!!
void put_single_texture(unsigned char* data, unsigned char* buffer, 
        int x, int y, int color_level)
{
    int i, j;
    unsigned char* t = data;
    unsigned char* b = buffer + x + y * 320;

    for (i = 0; i < 64; i++) {
        for (j = 0; j < 64; j++) {
            // draw every second pixel
            *(b++) = (*(t += 2)) + (color_level << 5);
        }
        b += 320 - 128 / 2;
        // skip one line
        t += 128;
    }
}

void display_palette(unsigned char* texturedata, unsigned char* buffer)
{
    int i, j, k;

    i = j = 0;

    for (k = 0; k < 8; k++) {
        put_single_texture(texturedata, buffer, i * 64, j * 64, k);
        i++;
        if (i > 4) {
            i = 0;
            j++;
        }
    }
}

void do_bump_mapping()
{
    int is_success = 0;

    drawcontext_t dc;
    object3d_t objcube;
    vector3d_t light = { -1, 0, 0 };
    texturemanager_t tm;

    unsigned char palette[768];

    is_success = create_manager(&tm);

    if (is_success) {
        is_success = create_cube(&objcube, texturefiles, 6, &tm);
        //is_success = load_object3d(&objcube, "assets/models/torus.3d");
        //TODO handle error !
        //is_success = set_envmap(&objcube, "assets/envmaps/envmap.pcx", &tm);

        if (is_success) {
            reset_and_scale_object3d(&objcube, 100.f);
            save_txt_object3d(&objcube, "objcube.txt");
            objcube.adx = 1;
            objcube.ady = 1;
            objcube.adz = 2;
                
            printf("All good. Press any key to start...\n");
            get_key_code();
        
            is_success = create_context(&dc, objcube.faces[0].mapper.texture->bitmap.palette);
            //is_success = create_context(&dc, asmshade_palette);
            /*
            create_phong_palette(palette);
            memcpy(palette, asmshade_palette, 768);
            compute_palette_levels(objcube.textures[0].palette, palette, 32);
            is_success = create_context(&dc, palette);
            */
            if (is_success) {
                /*
                display_palette(objcube.textures[0].data, dc.framebuffer);
                flip_buffer(&dc);
                get_key_code();
                */
                main_loop(&objcube, &light, &dc);
        
                destroy_context(&dc);
            }
        
            unload_object3d(&objcube);
        }
        destroy_manager(&tm);
    }

    if (!is_success) {
        //TODO 
    }
}
