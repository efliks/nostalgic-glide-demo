#ifndef _TEMPLATE_H
#define _TEMPLATE_H

#include "math3d.h"

typedef struct
{
    int s1, t1, s2, t2, s3, t3, texture_idx;
} mappertempl_t;

typedef struct
{
    int v1, v2, v3;
    mappertempl_t mapper;
} facetempl_t;

extern const point3d_t cubepoints[];

extern const facetempl_t cubefaces[];

#endif // _TEMPLATE_H
