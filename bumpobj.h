#ifndef _OBJ3D_H
#define _OBJ3D_H

#include "math3d.h"
#include "texture.h"

typedef struct
{
    int s1, t1, s2, t2, s3, t3;
    texture_t* texture;
} texcoord_t, mapper_t;

/*
typedef struct
{
    int v1, v2, v3;
} face_t;

typedef struct
{
    face_t* faces;
    point3d_t *points, *rotated_points;
    point2d_t* translated_points;
    texcoord_t* texcoords;
    texture_t* textures;
    vector3d_t *point_normals, *face_normals;

    float* facedepths;
    int* faceorder;

    int n_points, n_faces, n_textures;

    float x0, y0, z0;
    unsigned int ax, ay, az, adx, ady, adz;
} object3d_t;
*/

typedef struct
{
    point3d_t point;
    point3d_t rotated_point;
    point2d_t translated_point;
    vector3d_t normal;
    vector3d_t rotated_normal;
} vertexdata_t;

typedef struct
{
    vector3d_t face_normal;
    vector3d_t rotated_normal;
    mapper_t mapper;

    vertexdata_t *v1, *v2, *v3;
} facedata_t;

typedef struct
{
    float depth;
    facedata_t* face;
} faceorder_t;

typedef struct
{
    vertexdata_t* vertices;
    facedata_t* faces;
    faceorder_t* faceorder;
    texture_t* textures;

    int numpoints, numfaces, numtextures, numvisible;

    float x0, y0, z0;
    unsigned int ax, ay, az, adx, ady, adz;
} object3d_t;

void sort_faces(object3d_t *, vector3d_t *);

void update_object3d(object3d_t *);

void reset_object3d(object3d_t *);
void reset_and_scale_object3d(object3d_t *, float);

int create_cube(object3d_t *, char **, int);
void unload_object3d(object3d_t *);

void save_txt_object3d(object3d_t *, char *);

#endif  // _3DOBJ_H
