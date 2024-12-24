
#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "texman.h"

// Source: https://stackoverflow.com/questions/743939/a-minimal-hash-function-for-c
unsigned long compute_fnv1a(const uint8_t* data, size_t size)
{
    size_t i;
    uint32_t h = 2166136261UL;

    for (i = 0; i < size; i++) {
        h ^= data[i];
        h *= 16777619;
    }

    return (unsigned long)h;
}

int is_space_for_texture(texturemanager_t* tm)
{
    int is_good = 1;
    cachedtexture_t* newt;

    if ((tm->numtextures + 1) > tm->_numallocated) {
        // reallocate
        newt = (cachedtexture_t *)malloc(sizeof(cachedtexture_t) * 2 * tm->_numallocated);
        is_good = (newt != NULL) ? 1 : 0;
        if (is_good) {
            memcpy(newt, tm->textures, sizeof(cachedtexture_t) * tm->numtextures);
            free(tm->textures);
            tm->textures = newt;
            tm->_numallocated = 2 * tm->_numallocated;
        }
    }

    return is_good;
}

texture_t* get_texture(char* filename, texturemanager_t* tm)
{
    int i;
    unsigned long texture_id;
    cachedtexture_t *t;

    texture_id = compute_fnv1a(filename, strlen(filename));

    t = tm->textures;
    for (i = 0; i < tm->numtextures; i++, t++) {
        if (t->texture_id == texture_id) {
            return &t->texture;
        }
    }

    if (is_space_for_texture(tm)) {
        // pick last
        t = &tm->textures[tm->numtextures];
        
        if (load_texture(&t->texture, filename)) {
            t->texture_id = texture_id;
            tm->numtextures++;
            return &t->texture;
        }
    }

    printf("Error loading texture: %s\n", filename);

    return NULL;
}

int create_manager(texturemanager_t* tm)
{
    tm->numtextures = 0;

    // preallocate some textures for a start
    //FIXME!!!
    tm->_numallocated = 10;
    tm->textures = (cachedtexture_t *)malloc(sizeof(cachedtexture_t) * tm->_numallocated);
    if (tm->textures != NULL) {
        return 1;
    }

    return 0;
}

void destroy_manager(texturemanager_t* tm)
{
    int i;
    cachedtexture_t* t = tm->textures;

    i = tm->numtextures;
    while (tm->numtextures > 0) {
        unload_texture(&t->texture);
        tm->numtextures--;
        t++;
    }

    free(tm->textures);
}
