#ifndef _VGA_H
#define _VGA_H

void set_mode13h();
void unset_mode13h();

void retrace();

void copy_buffer(unsigned char *);
void clear_buffer(unsigned char *);

void set_palette(unsigned char *);
void dump_palette(unsigned char *);
void reset_palette();

#endif // _VGA_H
