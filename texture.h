#ifndef _TEXTURE_H
#define _TEXTURE_H

typedef enum { TEXTURE_SOFT, TEXTURE_GLIDE } texturetype_t;

typedef struct
{
    int width, height;
    unsigned char *data, *palette;
} bitmap_t;

typedef struct
{
    void* data;
    int is_in_tmu;
    unsigned long tmu_memory_addr;
} glidetexture_t;

typedef struct
{
    union {
        bitmap_t bitmap;
        glidetexture_t glidetexture;
    };

    texturetype_t texturetype;
} texture_t;

int load_texture(texture_t *, char *);
void unload_texture(texture_t *);

#endif // _TEXTURE_H
