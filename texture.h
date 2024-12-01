#ifndef _TEXTURE_H
#define _TEXTURE_H

typedef struct
{
    int width, height;
    unsigned char *data, *palette;
} texture_t;

int load_texture(texture_t *, char *);
void unload_texture(texture_t *);

#endif // _TEXTURE_H
