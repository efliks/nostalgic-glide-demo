
#include "bumptri.h"

#define SHIFT_CONST 6

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

void flat_triangle(int x1, int y1, int x2, int y2, int x3, int y3,
    unsigned char color, unsigned char* buffer)
{
    long dx12, dx13, dx23, scan_x1, scan_x2;
    int i;

    if (y1 > y2) {
        i = y1;
        y1 = y2;
        y2 = i;

        i = x1;
        x1 = x2;
        x2 = i;
    }
    if (y2 > y3) {
        i = y2;
        y2 = y3;
        y3 = i;

        i = x2;
        x2 = x3;
        x3 = i;
    }
    if (y1 > y2) {
        i = y1;
        y1 = y2;
        y2 = i;

        i = x1;
        x1 = x2;
        x2 = i;
    }

    if ((i = y2 - y1) != 0)
        dx12 = ((x2 - x1) << SHIFT_CONST) / i;
    else
        dx12 = 0;

    if ((i = y3 - y1) != 0)
        dx13 = ((x3 - x1) << SHIFT_CONST) / i;
    else
        dx13 = 0;

    if ((i = y3 - y2) != 0)
        dx23 = ((x3 - x2) << SHIFT_CONST) / i;
    else
        dx23 = 0;

    scan_x1 = scan_x2 = x1 << SHIFT_CONST;
    for (i = y1; i < y2; i++) {
        flat_line(scan_x1 >> SHIFT_CONST,
            scan_x2 >> SHIFT_CONST, i, color, buffer);

        scan_x1 += dx13;
        scan_x2 += dx12;
    }

    scan_x2 = x2 << SHIFT_CONST;
    for (i = y2; i < y3; i++) {
        flat_line(scan_x1 >> SHIFT_CONST,
            scan_x2 >> SHIFT_CONST, i, color, buffer);

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

void gouraud_triangle(int x1, int y1, int x2, int y2, int x3, int y3,
    unsigned char c1, unsigned char c2, unsigned char c3,
    unsigned char* buffer)
{
    long dx12, dx13, dx23, scan_x1, scan_x2;
    long dc12, dc13, dc23, scan_c1, scan_c2;
    int i;

    if (y1 > y2) {
        i = y1;
        y1 = y2;
        y2 = i;

        i = x1;
        x1 = x2;
        x2 = i;

        i = c1;
        c1 = c2;
        c2 = i;
    }
    if (y2 > y3) {
        i = y2;
        y2 = y3;
        y3 = i;

        i = x2;
        x2 = x3;
        x3 = i;

        i = c2;
        c2 = c3;
        c3 = i;
    }
    if (y1 > y2) {
        i = y1;
        y1 = y2;
        y2 = i;

        i = x1;
        x1 = x2;
        x2 = i;

        i = c1;
        c1 = c2;
        c2 = i;
    }

    if ((i = y2 - y1) != 0) {
        dx12 = ((x2 - x1) << SHIFT_CONST) / i;
        dc12 = ((c2 - c1) << SHIFT_CONST) / i;
    } else {
        dx12 = 0;
        dc12 = 0;
    }

    if ((i = y3 - y1) != 0) {
        dx13 = ((x3 - x1) << SHIFT_CONST) / i;
        dc13 = ((c3 - c1) << SHIFT_CONST) / i;
    } else {
        dx13 = 0;
        dc13 = 0;
    }

    if ((i = y3 - y2) != 0) {
        dx23 = ((x3 - x2) << SHIFT_CONST) / i;
        dc23 = ((c3 - c2) << SHIFT_CONST) / i;
    } else {
        dx23 = 0;
        dc23 = 0;
    }

    scan_x1 = scan_x2 = x1 << SHIFT_CONST;
    scan_c1 = scan_c2 = c1 << SHIFT_CONST;

    for (i = y1; i < y2; i++) {
        gouraud_line(scan_x1 >> SHIFT_CONST, scan_x2 >> SHIFT_CONST, i,
            scan_c1 >> SHIFT_CONST, scan_c2 >> SHIFT_CONST, buffer);

        scan_x1 += dx13;
        scan_x2 += dx12;
        scan_c1 += dc13;
        scan_c2 += dc12;
    }

    scan_x2 = x2 << SHIFT_CONST;
    scan_c2 = c2 << SHIFT_CONST;

    for (i = y2; i < y3; i++) {
        gouraud_line(scan_x1 >> SHIFT_CONST, scan_x2 >> SHIFT_CONST, i,
            scan_c1 >> SHIFT_CONST, scan_c2 >> SHIFT_CONST, buffer);

        scan_x1 += dx13;
        scan_x2 += dx23;
        scan_c1 += dc13;
        scan_c2 += dc23;
    }
}

static void textured_line(int x1, int x2, int y,
    int tx1, int ty1, int tx2, int ty2,
    unsigned char* texture, unsigned char* buffer)
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
        *ofs = texture[(curr_x >> SHIFT_CONST) + ((curr_y >> SHIFT_CONST) << 7)];
        ofs++;

        curr_x += delta_x;
        curr_y += delta_y;
    }
}

void textured_triangle(int x1, int y1, int x2, int y2, int x3, int y3,
    int tx1, int ty1, int tx2, int ty2, int tx3, int ty3,
    unsigned char* texture, unsigned char* buffer)
{
    long dx12, dx13, dx23, curr_x1, curr_x2;
    long tdx12, tdy12, tdx13, tdy13, tdx23, tdy23, scan_x1, scan_y1, scan_x2, scan_y2;
    int i;

    if (y1 > y2) {
        i = y1;
        y1 = y2;
        y2 = i;

        i = x1;
        x1 = x2;
        x2 = i;

        i = ty1;
        ty1 = ty2;
        ty2 = i;

        i = tx1;
        tx1 = tx2;
        tx2 = i;
    }
    if (y2 > y3) {
        i = y2;
        y2 = y3;
        y3 = i;

        i = x2;
        x2 = x3;
        x3 = i;

        i = ty2;
        ty2 = ty3;
        ty3 = i;

        i = tx2;
        tx2 = tx3;
        tx3 = i;
    }
    if (y1 > y2) {
        i = y1;
        y1 = y2;
        y2 = i;

        i = x1;
        x1 = x2;
        x2 = i;

        i = ty1;
        ty1 = ty2;
        ty2 = i;

        i = tx1;
        tx1 = tx2;
        tx2 = i;
    }

    if ((i = y2 - y1) != 0) {
        dx12 = ((x2 - x1) << SHIFT_CONST) / i;
        tdx12 = ((tx2 - tx1) << SHIFT_CONST) / i;
        tdy12 = ((ty2 - ty1) << SHIFT_CONST) / i;
    } else {
        dx12 = 0;
        tdx12 = 0;
        tdy12 = 0;
    }

    if ((i = y3 - y1) != 0) {
        dx13 = ((x3 - x1) << SHIFT_CONST) / i;
        tdx13 = ((tx3 - tx1) << SHIFT_CONST) / i;
        tdy13 = ((ty3 - ty1) << SHIFT_CONST) / i;
    } else {
        dx13 = 0;
        tdx13 = 0;
        tdy13 = 0;
    }

    if ((i = y3 - y2) != 0) {
        dx23 = ((x3 - x2) << SHIFT_CONST) / i;
        tdx23 = ((tx3 - tx2) << SHIFT_CONST) / i;
        tdy23 = ((ty3 - ty2) << SHIFT_CONST) / i;
    } else {
        dx23 = 0;
        tdx23 = 0;
        tdy23 = 0;
    }

    curr_x1 = curr_x2 = x1 << SHIFT_CONST;
    scan_x1 = scan_x2 = tx1 << SHIFT_CONST;
    scan_y1 = scan_y2 = ty1 << SHIFT_CONST;

    for (i = y1; i < y2; i++) {
        textured_line(curr_x1 >> SHIFT_CONST, curr_x2 >> SHIFT_CONST, i,
            scan_x1 >> SHIFT_CONST, scan_y1 >> SHIFT_CONST,
            scan_x2 >> SHIFT_CONST, scan_y2 >> SHIFT_CONST, texture, buffer);

        curr_x1 += dx13;
        curr_x2 += dx12;

        scan_x1 += tdx13;
        scan_y1 += tdy13;
        scan_x2 += tdx12;
        scan_y2 += tdy12;
    }

    curr_x2 = x2 << SHIFT_CONST;
    scan_x2 = tx2 << SHIFT_CONST;
    scan_y2 = ty2 << SHIFT_CONST;

    for (i = y2; i < y3; i++) {
        textured_line(curr_x1 >> SHIFT_CONST, curr_x2 >> SHIFT_CONST, i,
            scan_x1 >> SHIFT_CONST, scan_y1 >> SHIFT_CONST,
            scan_x2 >> SHIFT_CONST, scan_y2 >> SHIFT_CONST, texture, buffer);

        curr_x1 += dx13;
        curr_x2 += dx23;

        scan_x1 += tdx13;
        scan_y1 += tdy13;
        scan_x2 += tdx23;
        scan_y2 += tdy23;
    }
}
