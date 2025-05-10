#ifndef _PTPOLY2_H
#define _PTPOLY2_H

typedef struct {
    float x1, y1, z1;
    float x2, y2, z2;
    float x3, y3, z3;
    float u1, v1;
    float u2, v2;
    float u3, v3;
    char* texture;
} TPolytri;

void drawtpolyperspdivsubtri(TPolytri *, int, char *);

void drawtpolysubtri(TPolytri *, int, char *);

#endif // _PTPOLY2_H
