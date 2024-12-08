/*****************************************
  Mroczna Harmonia

  low.c   Low level routines to access hardware

  (C) 2001, 2023  M. Feliks
*****************************************/

#include <dos.h>
#include <i86.h>
#include <conio.h>
#include <string.h> // for memset

#ifdef __386__
#define DO_INT(i, r_in, r_out) int386(i, r_in, r_out)
#define VGA_MEMORY 0xA0000

#else
#define DO_INT(i, r_in, r_out) int86(i, r_in, r_out)
#define VGA_MEMORY 0xA0000000L

#endif

static unsigned char default_palette[768];


int is_key_pressed()
{
    return kbhit() ? 1 : 0;
}

char get_key_code()
{
    return getch();
}

void set_mode13h()
{
    union REGS regs;

    regs.w.ax = 0x13;
    DO_INT(0x10, &regs, &regs);

    //dump_palette(default_palette);
}

void unset_mode13h()
{
    union REGS regs;

    regs.w.ax = 0x3;
    DO_INT(0x10, &regs, &regs);
}

void copy_buffer(unsigned char* buffer)
{
    memcpy((unsigned char *)VGA_MEMORY, buffer, 64000);
}

void clear_buffer(unsigned char* buffer)
{
    memset(buffer, 0, 64000);
}

void retrace()
{
    while (!(inp(0x03da) & 8))
        ;
    while ((inp(0x03da) & 8))
        ;
}

void set_palette(unsigned char* my_pal)
{
    int i;
        
    outp(0x03c8, 0);
    for (i = 0; i < 768; i++) {
        outp(0x03c9, my_pal[i]);
    }
}

void dump_palette(unsigned char* my_pal)
{
    int i;
        
    outp(0x03c7, 0);
    for (i = 0; i < 768; i++) {
        my_pal[i] = inp(0x03c9);
    }
}

void reset_palette()
{
    set_palette(default_palette);
}
