
#include <math.h>
#include "math3d.h"

#define PERSPECTIVE 256.f

float sine[MAX_DEGS], cosine[MAX_DEGS];


void compute_lookup_tables()
{
    int i;
    float y;

    for (i = 0; i < MAX_DEGS; i++) {
        y = i * 3.14159265358979323846 / (MAX_DEGS / 2);

        sine[i] = sin(y);
        cosine[i] = cos(y);
    }
}

void compute_translated_point(point3d_t* p, point2d_t* o)
{
    float div;

    div = p->x + PERSPECTIVE;
    o->x = p->y * PERSPECTIVE / div;
    o->y = -p->z * PERSPECTIVE / div;
}

void compute_rotation_matrix(matrix_t m, int ax, int ay, int az)
{
    ax &= (MAX_DEGS - 1);
    ay &= (MAX_DEGS - 1);
    az &= (MAX_DEGS - 1);

    m[0][0] = cosine[ay] * cosine[az];
    m[1][0] = sine[ax] * sine[ay] * cosine[az] + cosine[ax] * (-sine[az]);
    m[2][0] = cosine[ax] * sine[ay] * cosine[az] + (-sine[ax]) * (-sine[az]);
    m[0][1] = cosine[ay] * sine[az];
    m[1][1] = sine[ax] * sine[ay] * sine[az] + cosine[ax] * cosine[az];
    m[2][1] = cosine[ax] * sine[ay] * sine[az] + (-sine[ax]) * cosine[az];
    m[0][2] = -sine[ay];
    m[1][2] = cosine[ay] * sine[ax];
    m[2][2] = cosine[ax] * cosine[ay];
}

void transform_vector(vector3d_t* v, matrix_t m, vector3d_t* o)
{
    o->x = v->x * m[0][0] + v->y * m[1][0] + v->z * m[2][0];
    o->y = v->x * m[0][1] + v->y * m[1][1] + v->z * m[2][1];
    o->z = v->x * m[0][2] + v->y * m[1][2] + v->z * m[2][2];
}

void copy_vector(vector3d_t* o, vector3d_t* v)
{
    o->x = v->x;
    o->y = v->y;
    o->z = v->z;
}

void scale_vector(float s, vector3d_t* v, vector3d_t* o)
{
    o->x = s * v->x;
    o->y = s * v->y;
    o->z = s * v->z;
}

void add_vector(vector3d_t* a, vector3d_t* b, vector3d_t* o)
{
    o->x = a->x + b->x;
    o->y = a->y + b->y;
    o->z = a->z + b->z;
}

void subtract_vector(vector3d_t* a, vector3d_t* b, vector3d_t* o)
{
    o->x = a->x - b->x;
    o->y = a->y - b->y;
    o->z = a->z - b->z;
}

void cross_product(vector3d_t* o, vector3d_t* a, vector3d_t* b)
{
    o->x = a->y * b->z - a->z * b->y;
    o->y = a->z * b->x - a->x * b->z;
    o->z = a->x * b->y - a->y * b->x;
}

float dot_product(vector3d_t* a, vector3d_t* b)
{
    return a->x * b->x + a->y * b->y + a->z * b->z;
}

float vector_len(vector3d_t* v)
{
    return sqrt(v->x * v->x + v->y * v->y + v->z * v->z);
}

float normalize_vector(vector3d_t* v)
{
    float s, t;

    s = sqrt(v->x * v->x + v->y * v->y + v->z * v->z);
    if (s != 0) {
        t = 1 / s;
        v->x *= t;
        v->y *= t;
        v->z *= t;
    }
    else {
        v->x = 0;
        v->y = 0;
        v->z = 0;
    }

    return s;
}
