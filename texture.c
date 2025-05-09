
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

int create_envmap_texture(texture_t* texture, config_envmap_t* cfg)
{
    int is_success;

    is_success = create_envmap(&texture->bitmap, cfg, cfg->is_auto);
    if (is_success) {
        texture->texturetype = TEXTURE_SOFT;
        printf("Computed %dx%d envmap\n", cfg->size, cfg->size);
    }

    return is_success;
}

void unload_texture(texture_t* texture)
{
    unload_bitmap(&texture->bitmap);
}
