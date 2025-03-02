
#include "bumptri.h"

#define SHIFT_CONST 6

static void swap_vertex(vertex_t* v1, vertex_t* v2)
{
    int swap;
    unsigned char swapc;

    swap = v1->x;
    v1->x = v2->x;
    v2->x = swap;

    swap = v1->y;
    v1->y = v2->y;
    v2->y = swap;

    swap = v1->tx;
    v1->tx = v2->tx;
    v2->tx = swap;

    swap = v1->ty;
    v1->ty = v2->ty;
    v2->ty = swap;

    swapc = v1->c;
    v1->c = v2->c;
    v2->c = swap;
}

static void reorder_vertices(vertex_t* v1, vertex_t* v2, vertex_t* v3)
{
    if (v1->y > v2->y) {
        swap_vertex(v1, v2);
    }
    if (v2->y > v3->y) {
        swap_vertex(v2, v3);
    }
    if (v1->y > v2->y) {
        swap_vertex(v1, v2);
    }
}

static void flat_line(int x1, int x2, int y, unsigned char color,
    unsigned char* buffer)
{
    int i;
    unsigned char* ofs;

    if (y < 0 || y > 199 || x1 == x2) {
        return;
    }

    if (x1 > x2) {
        i = x1;
        x1 = x2;
        x2 = i;
    }

    ofs = buffer;
    ofs += ((y << 6) + (y << 8) + x1);

    i = 0;
    while ((i++) < (x2 - x1)) {
        *ofs = color;
        ofs++;
    }
}

void flat_triangle(vertex_t* v1, vertex_t* v2, vertex_t* v3, unsigned char color, unsigned char* buffer)
{
    long dx12, dx13, dx23, scan_x1, scan_x2;
    int y, dy;

    reorder_vertices(v1, v2, v3);

    dy = v2->y - v1->y;
    if (dy != 0) {
        dx12 = ((v2->x - v1->x) << SHIFT_CONST) / dy;
    }
    else {
        dx12 = 0;
    }

    dy = v3->y - v1->y;
    if (dy != 0) {
        dx13 = ((v3->x - v1->x) << SHIFT_CONST) / dy;
    }
    else {
        dx13 = 0;
    }

    dy = v3->y - v2->y;
    if (dy != 0) {
        dx23 = ((v3->x - v2->x) << SHIFT_CONST) / dy;
    }
    else {
        dx23 = 0;
    }

    scan_x1 = scan_x2 = v1->x << SHIFT_CONST;
    for (y = v1->y; y < v2->y; y++) {
        flat_line(scan_x1 >> SHIFT_CONST, 
            scan_x2 >> SHIFT_CONST, y, color, buffer);

        scan_x1 += dx13;
        scan_x2 += dx12;
    }

    scan_x2 = v2->x << SHIFT_CONST;
    for (y = v2->y; y < v3->y; y++) {
        flat_line(scan_x1 >> SHIFT_CONST, 
            scan_x2 >> SHIFT_CONST, y, color, buffer);

        scan_x1 += dx13;
        scan_x2 += dx23;
    }
}

static void gouraud_line(int x1, int x2, int y, unsigned char c1, unsigned char c2,
    unsigned char* buffer)
{
    long delta_c, curr_c;
    int i;
    unsigned char* ofs;

    if (y < 0 || y > 199 || x1 == x2) {
        return;
    }

    if (x1 > x2) {
        i = x1;
        x1 = x2;
        x2 = i;

        i = c1;
        c1 = c2;
        c2 = i;
    }

    if (x1 > 319 || x2 < 0) {
        return;
    }

    delta_c = ((c2 - c1) << SHIFT_CONST) / (x2 - x1);
    curr_c = c1 << SHIFT_CONST;

    if (x1 < 0) {
        curr_c += (-x1) * delta_c;
        x1 = 0;
    }
    if (x2 > 319) {
        x2 = 319;
    }

    ofs = buffer;
    ofs += ((y << 6) + (y << 8) + x1);

    i = 0;
    while ((i++) < (x2 - x1)) {
        *ofs = curr_c >> SHIFT_CONST;
        curr_c += delta_c;
        ofs++;
    }
}

void gouraud_triangle(vertex_t* v1, vertex_t* v2, vertex_t* v3, unsigned char* buffer)
{
    long dx12, dx13, dx23, scan_x1, scan_x2;
    long dc12, dc13, dc23, scan_c1, scan_c2;
    int y, dy;

    reorder_vertices(v1, v2, v3);

    dy = v2->y - v1->y;
    if (dy != 0) {
        dx12 = ((v2->x - v1->x) << SHIFT_CONST) / dy;
        dc12 = ((v2->c - v1->c) << SHIFT_CONST) / dy;
    }
    else {
        dx12 = 0;
        dc12 = 0;
    }

    dy = v3->y - v1->y;
    if (dy != 0) {
        dx13 = ((v3->x - v1->x) << SHIFT_CONST) / dy;
        dc13 = ((v3->c - v1->c) << SHIFT_CONST) / dy;
    }
    else {
        dx13 = 0;
        dc13 = 0;
    }

    dy = v3->y - v2->y;
    if (dy != 0) {
        dx23 = ((v3->x - v2->x) << SHIFT_CONST) / dy;
        dc23 = ((v3->c - v2->c) << SHIFT_CONST) / dy;
    }
    else {
        dx23 = 0;
        dc23 = 0;
    }

    scan_x1 = scan_x2 = v1->x << SHIFT_CONST;
    scan_c1 = scan_c2 = v1->c << SHIFT_CONST;

    for (y = v1->y; y < v2->y; y++) {
        gouraud_line(scan_x1 >> SHIFT_CONST, scan_x2 >> SHIFT_CONST, y,
            scan_c1 >> SHIFT_CONST, scan_c2 >> SHIFT_CONST, buffer);

        scan_x1 += dx13;
        scan_x2 += dx12;
        scan_c1 += dc13;
        scan_c2 += dc12;
    }

    scan_x2 = v2->x << SHIFT_CONST;
    scan_c2 = v2->c << SHIFT_CONST;
    for (y = v2->y; y < v3->y; y++) {
        gouraud_line(scan_x1 >> SHIFT_CONST, scan_x2 >> SHIFT_CONST, y,
            scan_c1 >> SHIFT_CONST, scan_c2 >> SHIFT_CONST, buffer);

        scan_x1 += dx13;
        scan_x2 += dx23;
        scan_c1 += dc13;
        scan_c2 += dc23;
    }
}

static void textured_line(int x1, int x2, int y,
    int tx1, int ty1, int tx2, int ty2,
    int shifty, unsigned char* texture, unsigned char* buffer)
{
    long delta_x, delta_y, curr_x, curr_y;
    int i, lenght;
    unsigned char* ofs;

    if (y < 0 || y > 199 || x1 == x2) {
        return;
    }

    if (x1 > x2) {
        i = x1;
        x1 = x2;
        x2 = i;

        i = tx1;
        tx1 = tx2;
        tx2 = i;

        i = ty1;
        ty1 = ty2;
        ty2 = i;
    }

    lenght = x2 - x1;

    delta_x = ((tx2 - tx1) << SHIFT_CONST) / lenght;
    curr_x = tx1 << SHIFT_CONST;

    delta_y = ((ty2 - ty1) << SHIFT_CONST) / lenght;
    curr_y = ty1 << SHIFT_CONST;
 
    if (x1 < 0) {
        curr_x += (-x1) * delta_x;
        curr_y += (-x1) * delta_y;
        x1 = 0;
    }
    if (x2 > 319) {
        x2 = 319;
    }

    ofs = buffer;
    ofs += ((y << 6) + (y << 8) + x1);

    for (i = x1; i < x2; i++) {
        *ofs = texture[(curr_x >> SHIFT_CONST) + ((curr_y >> SHIFT_CONST) << shifty)];
        ofs++;

        curr_x += delta_x;
        curr_y += delta_y;
    }
}

void textured_triangle(vertex_t* v1, vertex_t* v2, vertex_t* v3, int shifty, unsigned char* texture, unsigned char* buffer)
{
    long dx12, dx13, dx23, curr_x1, curr_x2;
    long tdx12, tdy12, tdx13, tdy13, tdx23, tdy23, scan_x1, scan_y1, scan_x2, scan_y2;
    int y, dy;

    reorder_vertices(v1, v2, v3);

    dy = v2->y - v1->y;
    if (dy != 0) {
        dx12 = ((v2->x - v1->x) << SHIFT_CONST) / dy;
        tdx12 = ((v2->tx - v1->tx) << SHIFT_CONST) / dy;
        tdy12 = ((v2->ty - v1->ty) << SHIFT_CONST) / dy;
    }
    else {
        dx12 = 0;
        tdx12 = 0;
        tdy12 = 0;
    }

    dy = v3->y - v1->y;
    if (dy != 0) {
        dx13 = ((v3->x - v1->x) << SHIFT_CONST) / dy;
        tdx13 = ((v3->tx - v1->tx) << SHIFT_CONST) / dy;
        tdy13 = ((v3->ty - v1->ty) << SHIFT_CONST) / dy;
    }
    else {
        dx13 = 0;
        tdx13 = 0;
        tdy13 = 0;
    }

    dy = v3->y - v2->y;
    if (dy != 0) {
        dx23 = ((v3->x - v2->x) << SHIFT_CONST) / dy;
        tdx23 = ((v3->tx - v2->tx) << SHIFT_CONST) / dy;
        tdy23 = ((v3->ty - v2->ty) << SHIFT_CONST) / dy;
    }
    else {
        dx23 = 0;
        tdx23 = 0;
        tdy23 = 0;
    }

    curr_x1 = curr_x2 = v1->x << SHIFT_CONST;
    scan_x1 = scan_x2 = v1->tx << SHIFT_CONST;
    scan_y1 = scan_y2 = v1->ty << SHIFT_CONST;

    for (y = v1->y; y < v2->y; y++) {
        textured_line(curr_x1 >> SHIFT_CONST, curr_x2 >> SHIFT_CONST, y,
            scan_x1 >> SHIFT_CONST, scan_y1 >> SHIFT_CONST,
            scan_x2 >> SHIFT_CONST, scan_y2 >> SHIFT_CONST, shifty, texture, buffer);

        curr_x1 += dx13;
        curr_x2 += dx12;

        scan_x1 += tdx13;
        scan_y1 += tdy13;
        scan_x2 += tdx12;
        scan_y2 += tdy12;
    }

    curr_x2 = v2->x << SHIFT_CONST;
    scan_x2 = v2->tx << SHIFT_CONST;
    scan_y2 = v2->ty << SHIFT_CONST;

    for (y = v2->y; y < v3->y; y++) {
        textured_line(curr_x1 >> SHIFT_CONST, curr_x2 >> SHIFT_CONST, y,
            scan_x1 >> SHIFT_CONST, scan_y1 >> SHIFT_CONST,
            scan_x2 >> SHIFT_CONST, scan_y2 >> SHIFT_CONST, shifty, texture, buffer);

        curr_x1 += dx13;
        curr_x2 += dx23;

        scan_x1 += tdx13;
        scan_y1 += tdy13;
        scan_x2 += tdx23;
        scan_y2 += tdy23;
    }
}
