#ifndef _MATH3D_H
#define _MATH3D_H

#define MAX_DEGS 512

typedef float matrix_t[3][3];

typedef struct
{
    float x, y, z;
} vector3d_t, point3d_t;

typedef struct
{
    float x, y;
} point2d_t;

extern float sine[MAX_DEGS], cosine[MAX_DEGS];


void compute_lookup_tables();

void compute_translated_point(point3d_t* p, point2d_t* o);

void compute_rotation_matrix(matrix_t m, int ax, int ay, int az);
void transform_vector(vector3d_t* v, matrix_t m, vector3d_t* o);

void copy_vector(vector3d_t* o, vector3d_t* v);
void scale_vector(float s, vector3d_t* v, vector3d_t* o);
void add_vector(vector3d_t* a, vector3d_t* b, vector3d_t* o);
void subtract_vector(vector3d_t* a, vector3d_t* b, vector3d_t* o);

float normalize_vector(vector3d_t* v);

void cross_product(vector3d_t* o, vector3d_t* a, vector3d_t* b);
float dot_product(vector3d_t* a, vector3d_t* b);

float vector_len(vector3d_t* v);

#endif // _MATH3D_H
