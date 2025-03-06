
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "bumpobj.h"
#include "raster.h"
#include "texture.h"
#include "texman.h"
#include "sys.h"

int main_loop(object3d_t* obj, vector3d_t* light_source, drawcontext_t* drawcontext)
{
    int do_loop = 1;
    int quit = 0;
    char key;

    while (do_loop) {
        update_object3d(obj);
        draw_object3d(obj, light_source, drawcontext);

        flip_buffer(drawcontext);

        timer_wait();

        if (is_key_pressed()) {
            key = get_key_code();
            if (key == 27) {
                // escape key pressed
                do_loop = 0;
                quit = 1;
            }
            else if (key == 32) {
                // space key pressed
                do_loop = 0;
            }
        }
    }

    return quit;
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

int initialize_cube(object3d_t* cubeobj, texturemanager_t* tm)
{
    int is_success;
#ifdef GLIDE_BUILD
    const textureconfig_t configs[20] = { { "assets/textures/camino2.pcx", LOAD_FILE }, { "assets/textures/floor_g.pcx", LOAD_FILE }, { "assets/textures/metal.pcx", LOAD_FILE }, { "assets/textures/trak_g.pcx", LOAD_FILE }, { "assets/textures/univ42.pcx", LOAD_FILE }, { "assets/textures/univ43.pcx", LOAD_FILE } };
#else
    const textureconfig_t configs[20] = { { "assets/textures/s_camino.pcx", LOAD_FILE }, { "assets/textures/s_floorg.pcx", LOAD_FILE }, { "assets/textures/s_metal.pcx", LOAD_FILE }, { "assets/textures/s_trakg.pcx", LOAD_FILE }, { "assets/textures/s_univ2.pcx", LOAD_FILE }, { "assets/textures/s_univ3.pcx", LOAD_FILE } };
#endif
    is_success = create_cube(cubeobj, configs, 6, tm);
    if (is_success) {
        reset_and_scale_object3d(cubeobj, 100.f);
        //save_txt_object3d(cubeobj, "cubeobj.txt");
        cubeobj->adx = 1;
        cubeobj->ady = 1;
        cubeobj->adz = 2;
    }

    return is_success;
}

int initialize_torus(object3d_t* torusobj, texturemanager_t* tm)
{
    int is_success;
#ifdef GLIDE_BUILD
  const textureconfig_t config = {"assets/envmaps/envmap.3df", LOAD_FILE};
#else
  const textureconfig_t config = {"assets/envmaps/envmap.pcx", LOAD_FILE};
#endif

    is_success = load_object3d(torusobj, "assets/models/torus.3d");
    if (is_success) {
        reset_and_scale_object3d(torusobj, 110.f);
        //save_txt_object3d(torusobj, "torusobj.txt");
        torusobj->adx = 1;
        torusobj->ady = 1;
        torusobj->adz = 2;

        is_success = set_envmap(torusobj, &config, tm);
        if (!is_success) {
            unload_object3d(torusobj);
        }
    }

    return is_success;
}

int do_bump_mapping()
{
    int is_success, quit = 0;

    drawcontext_t dc;
    object3d_t cubeobj, torusobj;
    vector3d_t light = { -1, 0, 0 };
    texturemanager_t tm;

    is_success = create_manager(&tm);

    if (is_success) {
        is_success = initialize_cube(&cubeobj, &tm);
        if (is_success) {
            is_success = initialize_torus(&torusobj, &tm);

            if (is_success) {
                printf("All good. Press any key to start...\n");
                get_key_code();
                
                is_success = create_context(&dc);
                /*
                create_phong_palette(palette);
                memcpy(palette, asmshade_palette, 768);
                compute_palette_levels(objcube.textures[0].palette, palette, 32);
                */
                if (is_success) {
                    /*
                    display_palette(objcube.textures[0].data, dc.framebuffer);
                    flip_buffer(&dc);
                    get_key_code();
                    */
                    while (!quit) {
                        set_context_palette(&dc, cubeobj.faces[0].mapper.texture->bitmap.palette);
                        set_draw_mode(&dc, MODE_TEXTURE);
                        quit = main_loop(&cubeobj, &light, &dc);
                    
                        if (!quit) {
                            set_context_palette(&dc, torusobj.faces[0].mapper.texture->bitmap.palette);
                            set_draw_mode(&dc, MODE_ENVMAP);
                            quit = main_loop(&torusobj, &light, &dc);
                        }
                    }

                    destroy_context(&dc);
                }
                unload_object3d(&torusobj);
            }
            unload_object3d(&cubeobj);
        }
        destroy_manager(&tm);
    }

    return is_success;
}
