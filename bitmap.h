#ifndef _BITMAP_H
#define _BITMAP_H

typedef struct
{
    int width, height;
    unsigned char *data, *palette;
} bitmap_t;

int load_bitmap(bitmap_t *, const char *);
int store_bitmap(bitmap_t *, const char *);
int store_bitmap_with_text(bitmap_t *, const char *, char *);
void unload_bitmap(bitmap_t *);
int create_empty_bitmap(bitmap_t *, int);

#endif // _BITMAP_H
