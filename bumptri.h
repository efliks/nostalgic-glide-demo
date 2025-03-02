#ifndef _BUMPTRI_H
#define _BUMPTRI_H

typedef struct
{
    int x, y;
    int tx, ty;
    unsigned char c;
} vertex_t;

void flat_triangle(vertex_t *, vertex_t *, vertex_t *, unsigned char, unsigned char *);
void gouraud_triangle(vertex_t *, vertex_t *, vertex_t *, unsigned char *);
void textured_triangle(vertex_t *, vertex_t *, vertex_t *, int, unsigned char *, unsigned char *);

#endif  // _BUMPTRI_H
