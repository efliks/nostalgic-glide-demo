#ifndef _TEXMAN_H
#define _TEXMAN_H

#include "texture.h"

typedef struct
{
    unsigned long texture_id;
    texture_t texture;
} cachedtexture_t;

typedef struct
{
    int numtextures, _numallocated;
    cachedtexture_t* textures;
} texturemanager_t;

texture_t* get_texture(const char *, texturemanager_t *);

int create_manager(texturemanager_t *);
void destroy_manager(texturemanager_t *);

#endif // _TEXMAN_H
