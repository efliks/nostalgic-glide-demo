
#include <stdio.h>

#include "texture.h"

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

void unload_texture(texture_t* texture)
{
    unload_bitmap(&texture->bitmap);
}

void copy_texture(texture_t* dest, texture_t* src)
{
    // shallow copy is good enough
    dest->bitmap.data = src->bitmap.data;
    dest->bitmap.palette = src->bitmap.palette;

    dest->texturetype = src->texturetype;
}
