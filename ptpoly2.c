
// Copyright (C) 1997 by Mikael Kalms and 3DReview

// Module copied and modified from :
//  https://www.lysator.liu.se/~mikaelk/doc/perspectivetexture

// Texturemapper with perspective correction at every Nth pixel (scanline
//	subdivision), subpixels and subtexels, uses floats all the way through
//	except for when drawing each N-pixel span

#include "ptpoly2.h"

static float dizdx, duizdx, dvizdx, dizdy, duizdy, dvizdy;
static float dizdxn, duizdxn, dvizdxn;
static float xa, xb, iza, uiza, viza;
static float dxdya, dxdyb, dizdya, duizdya, dvizdya;
static char* texture;

// Subdivision span-size

#define SUBDIVSHIFT 4
#define SUBDIVSIZE (1 << SUBDIVSHIFT)

static void drawtpolyperspdivsubtriseg(int y1, int y2, int shifty, char* screen);

void drawtpolyperspdivsubtri(TPolytri* poly, int shifty, char* screen)
{
    float x1, y1, x2, y2, x3, y3;
    float iz1, uiz1, viz1, iz2, uiz2, viz2, iz3, uiz3, viz3;
    float dxdy1, dxdy2, dxdy3;
    float tempf;
    float denom;
    float dy;
    int y1i, y2i, y3i;
    int side;

    // Shift XY coordinate system (+0.5, +0.5) to match the subpixeling
    //  technique

    x1 = poly->x1 + 0.5;
    y1 = poly->y1 + 0.5;
    x2 = poly->x2 + 0.5;
    y2 = poly->y2 + 0.5;
    x3 = poly->x3 + 0.5;
    y3 = poly->y3 + 0.5;

    // Calculate alternative 1/Z, U/Z and V/Z values which will be
    //  interpolated

    iz1 = 1 / poly->z1;
    iz2 = 1 / poly->z2;
    iz3 = 1 / poly->z3;
    uiz1 = poly->u1 * iz1;
    viz1 = poly->v1 * iz1;
    uiz2 = poly->u2 * iz2;
    viz2 = poly->v2 * iz2;
    uiz3 = poly->u3 * iz3;
    viz3 = poly->v3 * iz3;

    texture = poly->texture;

    // Sort the vertices in increasing Y order

#define swapfloat(x, y) \
    tempf = x;          \
    x = y;              \
    y = tempf;
    if (y1 > y2) {
        swapfloat(x1, x2);
        swapfloat(y1, y2);
        swapfloat(iz1, iz2);
        swapfloat(uiz1, uiz2);
        swapfloat(viz1, viz2);
    }
    if (y1 > y3) {
        swapfloat(x1, x3);
        swapfloat(y1, y3);
        swapfloat(iz1, iz3);
        swapfloat(uiz1, uiz3);
        swapfloat(viz1, viz3);
    }
    if (y2 > y3) {
        swapfloat(x2, x3);
        swapfloat(y2, y3);
        swapfloat(iz2, iz3);
        swapfloat(uiz2, uiz3);
        swapfloat(viz2, viz3);
    }
#undef swapfloat

    y1i = y1;
    y2i = y2;
    y3i = y3;

    // Skip poly if it's too thin to cover any pixels at all

    if ((y1i == y2i && y1i == y3i)
        || ((int)x1 == (int)x2 && (int)x1 == (int)x3))
        return;

    // Calculate horizontal and vertical increments for UV axes (these
    //  calcs are certainly not optimal, although they're stable
    //  (handles any dy being 0)

    denom = ((x3 - x1) * (y2 - y1) - (x2 - x1) * (y3 - y1));

    if (!denom) // Skip poly if it's an infinitely thin line
        return;

    denom = 1 / denom; // Reciprocal for speeding up
    dizdx = ((iz3 - iz1) * (y2 - y1) - (iz2 - iz1) * (y3 - y1)) * denom;
    duizdx = ((uiz3 - uiz1) * (y2 - y1) - (uiz2 - uiz1) * (y3 - y1)) * denom;
    dvizdx = ((viz3 - viz1) * (y2 - y1) - (viz2 - viz1) * (y3 - y1)) * denom;
    dizdy = ((iz2 - iz1) * (x3 - x1) - (iz3 - iz1) * (x2 - x1)) * denom;
    duizdy = ((uiz2 - uiz1) * (x3 - x1) - (uiz3 - uiz1) * (x2 - x1)) * denom;
    dvizdy = ((viz2 - viz1) * (x3 - x1) - (viz3 - viz1) * (x2 - x1)) * denom;

    // Horizontal increases for 1/Z, U/Z and V/Z which step one full span
    //  ahead

    dizdxn = dizdx * SUBDIVSIZE;
    duizdxn = duizdx * SUBDIVSIZE;
    dvizdxn = dvizdx * SUBDIVSIZE;

    // Calculate X-slopes along the edges

    if (y2 > y1)
        dxdy1 = (x2 - x1) / (y2 - y1);
    if (y3 > y1)
        dxdy2 = (x3 - x1) / (y3 - y1);
    if (y3 > y2)
        dxdy3 = (x3 - x2) / (y3 - y2);

    // Determine which side of the poly the longer edge is on

    side = dxdy2 > dxdy1;

    if (y1 == y2)
        side = x1 > x2;
    if (y2 == y3)
        side = x3 > x2;

    if (!side) // Longer edge is on the left side
    {
        // Calculate slopes along left edge

        dxdya = dxdy2;
        dizdya = dxdy2 * dizdx + dizdy;
        duizdya = dxdy2 * duizdx + duizdy;
        dvizdya = dxdy2 * dvizdx + dvizdy;

        // Perform subpixel pre-stepping along left edge

        dy = 1 - (y1 - y1i);
        xa = x1 + dy * dxdya;
        iza = iz1 + dy * dizdya;
        uiza = uiz1 + dy * duizdya;
        viza = viz1 + dy * dvizdya;

        if (y1i < y2i) // Draw upper segment if possibly visible
        {
            // Set right edge X-slope and perform subpixel pre-
            //  stepping

            xb = x1 + dy * dxdy1;
            dxdyb = dxdy1;

            drawtpolyperspdivsubtriseg(y1i, y2i, shifty, screen);
        }
        if (y2i < y3i) // Draw lower segment if possibly visible
        {
            // Set right edge X-slope and perform subpixel pre-
            //  stepping

            xb = x2 + (1 - (y2 - y2i)) * dxdy3;
            dxdyb = dxdy3;

            drawtpolyperspdivsubtriseg(y2i, y3i, shifty, screen);
        }
    } else // Longer edge is on the right side
    {
        // Set right edge X-slope and perform subpixel pre-stepping

        dxdyb = dxdy2;
        dy = 1 - (y1 - y1i);
        xb = x1 + dy * dxdyb;

        if (y1i < y2i) // Draw upper segment if possibly visible
        {
            // Set slopes along left edge and perform subpixel
            //  pre-stepping

            dxdya = dxdy1;
            dizdya = dxdy1 * dizdx + dizdy;
            duizdya = dxdy1 * duizdx + duizdy;
            dvizdya = dxdy1 * dvizdx + dvizdy;
            xa = x1 + dy * dxdya;
            iza = iz1 + dy * dizdya;
            uiza = uiz1 + dy * duizdya;
            viza = viz1 + dy * dvizdya;

            drawtpolyperspdivsubtriseg(y1i, y2i, shifty, screen);
        }
        if (y2i < y3i) // Draw lower segment if possibly visible
        {
            // Set slopes along left edge and perform subpixel
            //  pre-stepping

            dxdya = dxdy3;
            dizdya = dxdy3 * dizdx + dizdy;
            duizdya = dxdy3 * duizdx + duizdy;
            dvizdya = dxdy3 * dvizdx + dvizdy;
            dy = 1 - (y2 - y2i);
            xa = x2 + dy * dxdya;
            iza = iz2 + dy * dizdya;
            uiza = uiz2 + dy * duizdya;
            viza = viz2 + dy * dvizdya;

            drawtpolyperspdivsubtriseg(y2i, y3i, shifty, screen);
        }
    }
}

static void drawtpolyperspdivsubtriseg(int y1, int y2, int shifty, char* screen)
{
    char* scr;
    int x1, x2;
    int x, xcount;
    float z, dx;
    float iz, uiz, viz;
    int u1, v1, u2, v2, u, v, du, dv;

    while (y1 < y2) // Loop through all lines in segment
    {
        x1 = xa;
        x2 = xb;

        // Perform subtexel pre-stepping on 1/Z, U/Z and V/Z

        dx = 1 - (xa - x1);
        iz = iza + dx * dizdx;
        uiz = uiza + dx * duizdx;
        viz = viza + dx * dvizdx;

        scr = &screen[y1 * 320 + x1];

        // Calculate UV for the first pixel

        z = 65536 / iz;
        u2 = uiz * z;
        v2 = viz * z;

        // Length of line segment

        xcount = x2 - x1;

        while (xcount >= SUBDIVSIZE) // Draw all full-length
        { //  spans
            // Step 1/Z, U/Z and V/Z to the next span

            iz += dizdxn;
            uiz += duizdxn;
            viz += dvizdxn;

            u1 = u2;
            v1 = v2;

            // Calculate UV at the beginning of next span

            z = 65536 / iz;
            u2 = uiz * z;
            v2 = viz * z;

            u = u1;
            v = v1;

            // Calculate linear UV slope over span

            du = (u2 - u1) >> SUBDIVSHIFT;
            dv = (v2 - v1) >> SUBDIVSHIFT;

            x = SUBDIVSIZE;
            while (x--) // Draw span
            {
                // Copy pixel from texture to screen

                *(scr++) = texture[((((int)v) >> 16) << shifty) + (((int)u) >> 16)];

                // Step horizontally along UV axes

                u += du;
                v += dv;
            }

            xcount -= SUBDIVSIZE; // One span less
        }

        if (xcount) // Draw last, non-full-length span
        {
            // Step 1/Z, U/Z and V/Z to end of span

            iz += dizdx * xcount;
            uiz += duizdx * xcount;
            viz += dvizdx * xcount;

            u1 = u2;
            v1 = v2;

            // Calculate UV at end of span

            z = 65536 / iz;
            u2 = uiz * z;
            v2 = viz * z;

            u = u1;
            v = v1;

            // Calculate linear UV slope over span

            du = (u2 - u1) / xcount;
            dv = (v2 - v1) / xcount;

            while (xcount--) // Draw span
            {
                // Copy pixel from texture to screen

                *(scr++) = texture[((((int)v) >> 16) << shifty) + (((int)u) >> 16)];

                // Step horizontally along UV axes

                u += du;
                v += dv;
            }
        }

        // Step vertically along both edges

        xa += dxdya;
        xb += dxdyb;
        iza += dizdya;
        uiza += duizdya;
        viza += dvizdya;

        y1++;
    }
}

// Texturemapper with subpixels and subtexels, uses floats all the way
//  through

static float dudx, dvdx, dudy, dvdy;
static float ua, va;
static float dudya, dvdya;

static void drawtpolysubtriseg(int y1, int y2, int shifty, char* screen);

void drawtpolysubtri(TPolytri* poly, int shifty, char* screen)
{
    float x1, y1, x2, y2, x3, y3;
    float u1, v1, u2, v2, u3, v3;
    float dxdy1, dxdy2, dxdy3;
    float tempf;
    float denom;
    float dy;
    int y1i, y2i, y3i;
    int side;

    // Shift XY coordinate system (+0.5, +0.5) to match the subpixeling
    //  technique

    x1 = poly->x1 + 0.5;
    y1 = poly->y1 + 0.5;
    x2 = poly->x2 + 0.5;
    y2 = poly->y2 + 0.5;
    x3 = poly->x3 + 0.5;
    y3 = poly->y3 + 0.5;

    u1 = poly->u1;
    v1 = poly->v1;
    u2 = poly->u2;
    v2 = poly->v2;
    u3 = poly->u3;
    v3 = poly->v3;

    texture = poly->texture;

    // Sort the vertices in ascending Y order

#define swapfloat(x, y) \
    tempf = x;          \
    x = y;              \
    y = tempf;
    if (y1 > y2) {
        swapfloat(x1, x2);
        swapfloat(y1, y2);
        swapfloat(u1, u2);
        swapfloat(v1, v2);
    }
    if (y1 > y3) {
        swapfloat(x1, x3);
        swapfloat(y1, y3);
        swapfloat(u1, u3);
        swapfloat(v1, v3);
    }
    if (y2 > y3) {
        swapfloat(x2, x3);
        swapfloat(y2, y3);
        swapfloat(u2, u3);
        swapfloat(v2, v3);
    }
#undef swapfloat

    y1i = y1;
    y2i = y2;
    y3i = y3;

    // Skip poly if it's too thin to cover any pixels at all

    if ((y1i == y2i && y1i == y3i)
        || ((int)x1 == (int)x2 && (int)x1 == (int)x3))
        return;

    // Calculate horizontal and vertical increments for UV axes (these
    //  calcs are certainly not optimal, although they're stable
    //  (handles any dy being 0)

    denom = ((x3 - x1) * (y2 - y1) - (x2 - x1) * (y3 - y1));

    if (!denom) // Skip poly if it's an infinitely thin line
        return;

    denom = 1 / denom; // Reciprocal for speeding up
    dudx = ((u3 - u1) * (y2 - y1) - (u2 - u1) * (y3 - y1)) * denom;
    dvdx = ((v3 - v1) * (y2 - y1) - (v2 - v1) * (y3 - y1)) * denom;
    dudy = ((u2 - u1) * (x3 - x1) - (u3 - u1) * (x2 - x1)) * denom;
    dvdy = ((v2 - v1) * (x3 - x1) - (v3 - v1) * (x2 - x1)) * denom;

    // Calculate X-slopes along the edges

    if (y2 > y1)
        dxdy1 = (x2 - x1) / (y2 - y1);
    if (y3 > y1)
        dxdy2 = (x3 - x1) / (y3 - y1);
    if (y3 > y2)
        dxdy3 = (x3 - x2) / (y3 - y2);

    // Determine which side of the poly the longer edge is on

    side = dxdy2 > dxdy1;

    if (y1 == y2)
        side = x1 > x2;
    if (y2 == y3)
        side = x3 > x2;

    if (!side) // Longer edge is on the left side
    {
        // Calculate slopes along left edge

        dxdya = dxdy2;
        dudya = dxdya * dudx + dudy;
        dvdya = dxdya * dvdx + dvdy;

        // Perform subpixel pre-stepping along left edge

        dy = 1 - (y1 - y1i);
        xa = x1 + dy * dxdya;
        ua = u1 + dy * dudya;
        va = v1 + dy * dvdya;

        if (y1i < y2i) // Draw upper segment if possibly visible
        {
            // Set right edge X-slope and perform subpixel pre-
            //  stepping

            dxdyb = dxdy1;
            xb = x1 + dy * dxdyb;

            drawtpolysubtriseg(y1i, y2i, shifty, screen);
        }
        if (y2i < y3i) // Draw lower segment if possibly visible
        {
            // Set right edge X-slope and perform subpixel pre-
            //  stepping

            dxdyb = dxdy3;
            xb = x2 + (1 - (y2 - y2i)) * dxdyb;

            drawtpolysubtriseg(y2i, y3i, shifty, screen);
        }
    } else // Longer edge is on the right side
    {
        // Set right edge X-slope and perform subpixel pre-stepping

        dxdyb = dxdy2;
        dy = 1 - (y1 - y1i);
        xb = x1 + dy * dxdyb;

        if (y1i < y2i) // Draw upper segment if possibly visible
        {
            // Set slopes along left edge and perform subpixel
            //  pre-stepping

            dxdya = dxdy1;
            dudya = dxdya * dudx + dudy;
            dvdya = dxdya * dvdx + dvdy;
            xa = x1 + dy * dxdya;
            ua = u1 + dy * dudya;
            va = v1 + dy * dvdya;

            drawtpolysubtriseg(y1i, y2i, shifty, screen);
        }
        if (y2i < y3i) // Draw lower segment if possibly visible
        {
            // Set slopes along left edge and perform subpixel
            //  pre-stepping

            dxdya = dxdy3;
            dudya = dxdya * dudx + dudy;
            dvdya = dxdya * dvdx + dvdy;
            dy = 1 - (y2 - y2i);
            xa = x2 + dy * dxdya;
            ua = u2 + dy * dudya;
            va = v2 + dy * dvdya;

            drawtpolysubtriseg(y2i, y3i, shifty, screen);
        }
    }
}

static void drawtpolysubtriseg(int y1, int y2, int shifty, char* screen)
{
    char* scr;
    int x1, x2;
    float u, v, dx;

    while (y1 < y2) // Loop through all lines in the segment
    {
        x1 = xa;
        x2 = xb;

        // Perform subtexel pre-stepping on UV

        dx = 1 - (xa - x1);
        u = ua + dx * dudx;
        v = va + dx * dvdx;

        scr = &screen[y1 * 320 + x1];

        while (x1++ < x2) // Draw horizontal line
        {
            // Copy pixel from texture to screen

            *(scr++) = texture[((((int)v) & 0xff) << shifty) + (((int)u) & 0xff)];

            // Step UV horizontally

            u += dudx;
            v += dvdx;
        }

        // Step along both edges

        xa += dxdya;
        xb += dxdyb;
        ua += dudya;
        va += dvdya;

        y1++;
    }
}
