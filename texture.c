
#include <stdio.h>

#include "texture.h"
#include "envmap.h"

int load_texture(texture_t* texture, const char* filename) 
{
    int is_success;

    is_success = load_bitmap(&texture->bitmap, filename);
    if (is_success) {
        texture->texturetype = TEXTURE_SOFT;
        printf("Loaded texture: %s\n", filename);
    }

    return is_success;
}

//TODO Pass parameters
int create_envmap(texture_t* texture)
{
    int is_success = 0;
    //TODO
    int size = 128;

    if (create_empty_bitmap(&texture->bitmap, size)) {
        compute_envmap(texture->bitmap.data, size, 63., 1.15);
        compute_phong_palette(texture->bitmap.palette);
        printf("Computed %dx%d envmap\n", size, size);
        is_success = 1;
    }

    return is_success;
}

void unload_texture(texture_t* texture)
{
    unload_bitmap(&texture->bitmap);
}
