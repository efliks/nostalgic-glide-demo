
#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "texman.h"

// Source: https://stackoverflow.com/questions/743939/a-minimal-hash-function-for-c
static unsigned long compute_fnv1a(const uint8_t* data, size_t size)
{
    size_t i;
    uint32_t h = 2166136261UL;

    for (i = 0; i < size; i++) {
        h ^= data[i];
        h *= 16777619;
    }

    return (unsigned long)h;
}

static unsigned long compute_texture_id(const textureconfig_t* tc)
{
    unsigned long texture_id;
    static unsigned long envmap_id = (unsigned long)-1;

    switch (tc->type) {
    case LOAD_FILE:
        texture_id = compute_fnv1a(tc->file.filename, strlen(tc->file.filename));
        break;
    case COMPUTE_ENVMAP:
        texture_id = (--envmap_id);
        break;
    default:
        break;
    }

    return texture_id;
}

static int create_texture(texture_t* texture, const textureconfig_t* tc)
{
    int is_success = 0;

    switch (tc->type) {
    case LOAD_FILE:
        is_success = load_texture(texture, tc->file.filename);
        break;
    case COMPUTE_ENVMAP:
        //TODO
        break;
    default:
        break;
    }

    return is_success;
}

static cachedtexture_t* create_node(cachedtexture_t* parent, const textureconfig_t* tc, unsigned long texture_id)
{
    cachedtexture_t* node;
    texture_t texture;

    if (create_texture(&texture, tc)) {
        node = (cachedtexture_t *)malloc(sizeof(cachedtexture_t));
        if (node != NULL) {
            memcpy(&node->texture, &texture, sizeof(texture_t));
            node->texture_id = texture_id;
            node->parent = parent;
            node->child = NULL;
            // printf("Create texture: %d\n", node->texture_id);
            return node;
        }

        unload_texture(&texture);
    }

    return NULL;
}

texture_t* get_texture(const textureconfig_t* config, texturemanager_t* tm)
{
    int i;
    unsigned long texture_id;
    cachedtexture_t *t;

    texture_id = compute_texture_id(config);

    // create root node
    if (tm->root == NULL) {
        tm->root = create_node(NULL, config, texture_id);
        if (tm->root != NULL) {
            return &tm->root->texture;
        }
    }

    // search for existing texture
    t = tm->root;
    while (t != NULL) {
        if (t->texture_id == texture_id) {
            return &t->texture;
        }
        t = t->child;
    }

    // create new node
    t = tm->root;
    if (t != NULL) {
        while (t->child != NULL) {
            t = t->child;
        }
        
        t->child = create_node(t, config, texture_id);
        if (t->child != NULL) {
            return &t->child->texture;
        }
    }

    //FIXME Improve error handling
    if (config->type == LOAD_FILE) {
        printf("Error loading texture: %s\n", config->file.filename);
    }
    else {
        printf("Error creating texture.\n");
    }

    return NULL;
}

int create_manager(texturemanager_t* tm)
{
    tm->root = NULL;
    return 1;
}

void destroy_manager(texturemanager_t* tm)
{
    cachedtexture_t *t, *tp;

    t = tm->root;
    if (t != NULL) {
        while (t->child != NULL) {
            t = t->child;
        }

        while (t != NULL) {
            // printf("Free texture: %d\n", t->texture_id);
            unload_texture(&t->texture);
            tp = t;
            t = t->parent;
            free(tp);
        }
    }
}
