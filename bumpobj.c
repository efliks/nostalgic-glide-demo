
#include <stdio.h>
#include <stdlib.h>
#include <string.h>  // for memset

#include "bumpobj.h"
#include "template.h"

typedef struct
{
    int v1, v2, v3;
} facevertices_t;

/*
void sort_faces(object3d_t *);

void update_object3d(object3d_t *);

void reset_object3d(object3d_t *);
void reset_and_scale_object3d(object3d_t *, float);
*/

void compute_vertex_normals(object3d_t *);
void compute_face_normals(object3d_t* obj);

void create_face_from_template(facedata_t *, const facetempl_t *, vertexdata_t *, texture_t *);

void unload_all_textures(texture_t *, int);
int load_all_textures(object3d_t *, char **, int);

int create_from_template(object3d_t *, const point3d_t *, const facetempl_t *, char **, int, int, int);

//int create_cube(object3d_t *, char** texturefiles, int numtextures);

int allocate_object3d(object3d_t* obj);

int compare_faces(const void *, const void *);


int allocate_object3d(object3d_t* obj)
{
    int is_success = 0;

    obj->vertices = (vertexdata_t *)malloc(obj->numpoints * sizeof(vertexdata_t));

    if (obj->vertices) {
        obj->faces = (facedata_t *)malloc(obj->numfaces * sizeof(facedata_t));

        if (obj->faces) {
            obj->faceorder = (faceorder_t *)malloc(obj->numfaces * sizeof(faceorder_t));

            if (obj->faceorder) {
                obj->textures = (texture_t *)malloc(obj->numtextures * sizeof(texture_t));

                if (obj->textures) {
                    is_success = 1;
                }
            }
        }
    }

    if (!is_success) {
        unload_object3d(obj);
    }

    return is_success;
}

void unload_object3d(object3d_t *obj)
{
    if (obj->vertices) {
        free(obj->vertices);

        if (obj->faces) {
            free(obj->faces);

            if (obj->faceorder) {
                free(obj->faceorder);

                if (obj->textures) {
                    unload_all_textures(obj->textures, obj->numtextures);
                    free(obj->textures);
                }
            }
        }
    }
}

int create_cube(object3d_t *obj, char** texturefiles, int numtextures)
{
    return create_from_template(obj, cubepoints, cubefaces, texturefiles, 8, 12, numtextures);
}

int create_from_template(object3d_t *obj, const point3d_t* points, const facetempl_t* faces, char** texturefiles, int numpoints, int numfaces, int numtextures)
{
    int i, is_success;

    obj->numpoints = numpoints;
    obj->numfaces = numfaces;
    obj->numtextures = numtextures;

    is_success = allocate_object3d(obj);
    if (is_success) {
        for (i = 0; i < numpoints; i++) {
            copy_vector(&obj->vertices[i].point, &points[i]);
        }

        for (i = 0; i < numfaces; i++) {
            create_face_from_template(&obj->faces[i], &faces[i], obj->vertices, obj->textures);
        }

        is_success = load_all_textures(obj, texturefiles, numtextures);
        if (is_success) {
            compute_face_normals(obj);
            compute_vertex_normals(obj);
        }
    }

    if (!is_success) {
        unload_object3d(obj);
    }

    return is_success;
}

int load_all_textures(object3d_t *obj, char** texturefiles, int numtextures)
{
    int i, is_success = 1;

    for (i = 0; i < numtextures; i++) {
        is_success = load_texture(&obj->textures[i], texturefiles[i]);
        if (!is_success) {
            break;
        }
    }

    return is_success;
}

void unload_all_textures(texture_t* textures, int numtextures)
{
    int i;

    for (i = 0; i < numtextures; i++) {
        unload_texture(&textures[i]);
    }
}

void create_face_from_template(facedata_t* face, const facetempl_t* templ, vertexdata_t* vertices, texture_t* textures)
{
    face->v1 = &vertices[templ->v1];
    face->v2 = &vertices[templ->v2];
    face->v3 = &vertices[templ->v3];

    // Assume all textures are 128x128
    face->mapper.s1 = templ->mapper.s1 << 7;
    face->mapper.t1 = templ->mapper.t1 << 7;

    face->mapper.s2 = templ->mapper.s2 << 7;
    face->mapper.t2 = templ->mapper.t2 << 7;

    face->mapper.s3 = templ->mapper.s3 << 7;
    face->mapper.t3 = templ->mapper.t3 << 7;

    face->mapper.texture = &textures[templ->mapper.texture_idx];
}

void compute_face_normals(object3d_t* obj)
{
    int i;
    vector3d_t va, vb;
    
    facedata_t* f = obj->faces;

    for (i = 0; i < obj->numfaces; i++) {
        subtract_vector(&f->v2->point, &f->v1->point, &va);
        subtract_vector(&f->v3->point, &f->v2->point, &vb);

        cross_product(&f->face_normal, &va, &vb);
        normalize_vector(&f->face_normal);

        f++;
    }
}

void compute_vertex_normals(object3d_t *obj)
{
    int i, j, face_hit;
    vector3d_t N;

    facedata_t* f;
    vertexdata_t* v = obj->vertices;

    for (i = 0; i < obj->numpoints; i++) {

        N.x = 0;
        N.y = 0;
        N.z = 0;
        face_hit = 0;
        f = obj->faces;
        for (j = 0; j < obj->numfaces; j++) {
            if (f->v1 == v || f->v2 == v || f->v3 == v) {
                add_vector(&N, &f->face_normal, &N);
                face_hit++;
            }
            f++;
        }

        scale_vector(1 / (float)face_hit, &N, &N);
        copy_vector(&v->normal, &N);
        v++;
    }
}

void reset_object3d(object3d_t *obj)
{
    obj->x0 = 0;
    obj->y0 = 0;
    obj->z0 = 0;

    obj->ax = 0;
    obj->ay = 0;
    obj->az = 0;

    obj->adx = 0;
    obj->ady = 0;
    obj->adz = 0;
}

void reset_and_scale_object3d(object3d_t *obj, float scalefactor)
{
    int i;
    float maxd, currd, scale;

    vector3d_t center;
    vertexdata_t* v = obj->vertices;

    center.x = 0;
    center.y = 0;
    center.z = 0;
    for (i = 0; i < obj->numpoints; i++) {
        add_vector(&center, &v->point, &center);
        v++;
    }

    scale_vector(1 / (float)obj->numpoints, &center, &center);

    v = obj->vertices;
    for (i = 0; i < obj->numpoints; i++) {
        subtract_vector(&v->point, &center, &v->point);
        v++;
    }

    maxd = 0;
    v = obj->vertices;
    for (i = 0; i < obj->numpoints; i++) {
        currd = vector_len(&v->point);
        if (currd > maxd) {
            maxd = currd;
        }
        v++;
    }

    scale = scalefactor / maxd;
    v = obj->vertices;

    for (i = 0; i < obj->numpoints; i++) {
        scale_vector(scale, &v->point, &v->point);
        v++;
    }

    reset_object3d(obj);
}

void sort_faces(object3d_t *obj, vector3d_t* lookvector)
{
    int i;
    float x1, y1, x2, y2, x3, y3;

    facedata_t* f = obj->faces;
    faceorder_t* fo = obj->faceorder;

    obj->numvisible = 0;

    for (i = 0; i < obj->numfaces; i++) {
        x1 = f->v1->translated_point.x;
        y1 = f->v1->translated_point.y;
        
        x2 = f->v2->translated_point.x;
        y2 = f->v2->translated_point.y;
        
        x3 = f->v3->translated_point.x;
        y3 = f->v3->translated_point.y;

        if (((y1 - y3) * (x2 - x1) - (x1 - x3) * (y2 - y1)) <= 0) {
        //if (dot_product(&f->rotated_normal, lookvector) >= 0) {
            fo->face = f;
            fo->depth = f->v1->rotated_point.x;
            fo->depth += f->v2->rotated_point.x;
            fo->depth += f->v3->rotated_point.x;

            obj->numvisible++;
            fo++;
        }

        f++;
    }

    qsort(obj->faceorder, obj->numvisible, sizeof(faceorder_t), compare_faces);
}

int compare_faces(const void* a, const void* b)
{
    return ((faceorder_t *) a)->depth < ((faceorder_t *) b)->depth;
}

void update_object3d(object3d_t *obj)
{
    int i;
    matrix_t m;
    facedata_t* f = obj->faces;
    vertexdata_t* v = obj->vertices;

    obj->ax += obj->adx;
    obj->ay += obj->ady;
    obj->az += obj->adz;
    compute_rotation_matrix(m, obj->ax, obj->ay, obj->az);

    for (i = 0; i < obj->numfaces; i++) {
        transform_vector(&f->face_normal, m, &f->rotated_normal);
        f++;
    }

    for (i = 0; i < obj->numpoints; i++) {
        transform_vector(&v->point, m, &v->rotated_point);
        transform_vector(&v->normal, m, &v->rotated_normal);

        compute_translated_point(&v->rotated_point, &v->translated_point);

        v++;
    }
}

int find_idx(vertexdata_t* v, object3d_t* obj)
{
    int i;
    vertexdata_t* vo = obj->vertices;

    for (i = 0; i < obj->numpoints; i++) {
        if (v == vo) {
            return i;
        }
        vo++;
    }

    return -1;  // should never happen
}

void save_txt_object3d(object3d_t* obj, char* filename)
{
    FILE* fp;
    int i;

    fp = fopen(filename, "w");

    if (fp != NULL) {
        fprintf(fp, "%d\n%d\n", obj->numpoints, obj->numfaces);

        for (i = 0; i < obj->numpoints; i++) {
            fprintf(fp, "v %f,  %f,  %f\n", obj->vertices[i].point.x, obj->vertices[i].point.y, obj->vertices[i].point.z);
        }
        for (i = 0; i < obj->numfaces; i++) {
            fprintf(fp, "f %d,  %d,  %d\n", find_idx(obj->faces[i].v1, obj), find_idx(obj->faces[i].v2, obj), find_idx(obj->faces[i].v3, obj));
        }
        for (i = 0; i < obj->numpoints; i++) {
            fprintf(fp, "vn %f,  %f,  %f\n", obj->vertices[i].normal.x, obj->vertices[i].normal.y, obj->vertices[i].normal.z);
        }
        for (i = 0; i < obj->numfaces; i++) {
            fprintf(fp, "fn %f,  %f,  %f\n", obj->faces[i].face_normal.x, obj->faces[i].face_normal.y, obj->faces[i].face_normal.z);
        }

        fclose(fp);
    }
}
