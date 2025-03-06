#ifndef _TEXMAN_H
#define _TEXMAN_H

#include "texture.h"

typedef enum 
{
    LOAD_FILE,
    COMPUTE_ENVMAP
} configtype_t;

typedef struct
{
    const char* filename;
} config_file_t;

typedef struct
{
    union {
        config_file_t file;
    };

    configtype_t type;
} textureconfig_t;

typedef struct cachedtexture_s
{
    unsigned long texture_id;
    texture_t texture;
    struct cachedtexture_s *parent, *child;
} cachedtexture_t;

typedef struct
{
    cachedtexture_t* root;
} texturemanager_t;

texture_t* get_texture(const textureconfig_t *, texturemanager_t *);

int create_manager(texturemanager_t *);
void destroy_manager(texturemanager_t *);

#endif // _TEXMAN_H
