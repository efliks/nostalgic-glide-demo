
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

static cachedtexture_t* create_node(cachedtexture_t* parent, const char* filename, unsigned long texture_id)
{
    cachedtexture_t* node;
    texture_t texture;

    if (load_texture(&texture, filename)) {
        node = (cachedtexture_t *)malloc(sizeof(cachedtexture_t));
        if (node != NULL) {
            memcpy(&node->texture, &texture, sizeof(texture_t));
            node->texture_id = texture_id;
            node->parent = parent;
            node->child = NULL;
            // printf("Load texture: %d\n", node->texture_id);
            return node;
        }

        unload_texture(&texture);
    }

    return NULL;
}

texture_t* get_texture(const char* filename, texturemanager_t* tm)
{
    int i;
    unsigned long texture_id;
    cachedtexture_t *t;

    texture_id = compute_fnv1a(filename, strlen(filename));

    // create root node
    if (tm->root == NULL) {
        tm->root = create_node(NULL, filename, texture_id);
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
        
        t->child = create_node(t, filename, texture_id);
        if (t->child != NULL) {
            return &t->child->texture;
        }
    }

    printf("Error loading texture: %s\n", filename);

    return NULL;
}

int create_manager(texturemanager_t* tm)
{
    tm->numtextures = 0;
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
