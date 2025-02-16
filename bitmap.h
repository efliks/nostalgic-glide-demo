#ifndef _BITMAP_H
#define _BITMAP_H

typedef struct
{
    int width, height;
    unsigned char *data, *palette;
} bitmap_t;

int load_bitmap(bitmap_t *, const char *);
int store_bitmap(bitmap_t *, const char *);
void unload_bitmap(bitmap_t *);

#endif // _BITMAP_H
