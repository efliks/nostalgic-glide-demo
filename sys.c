
#include <dos.h>
#include <conio.h>

#include "sys.h"

#define TIMES_FASTER 15
#define WAIT_TICKS 8

// 18.206775005722132 ticks/s
#define DOS_DIVISOR 65535

// 273.101625085832 ticks/s
#define MY_DIVISOR DOS_DIVISOR / TIMES_FASTER

void interrupt (*old_clock_interrupt)();

volatile unsigned long timer = 0;


int is_key_pressed()
{
    return kbhit() ? 1 : 0;
}

char get_key_code()
{
    return getch();
}

void timer_wait()
{
    timer_custom_wait(WAIT_TICKS);
}

void timer_custom_wait(unsigned long ticks)
{
    static unsigned long timer_stop;

    while (timer < timer_stop) {
        // Reduce CPU load in VirtualBox from 100% to 65%
        _asm {
            hlt
        }
    }
    timer_stop = timer + ticks;
}

void interrupt clock_interrupt()
{
    timer++;

    if (timer % TIMES_FASTER == 0) {
        old_clock_interrupt();
    }
}

static void set_clock_divisor(short divisor)
{
    outp(0x43, 0x34);
    outp(0x40, divisor & 0xff);
    outp(0x40, (divisor >> 8) & 0xff);
}

void clock_init()
{
    old_clock_interrupt = _dos_getvect(0x1c);
    _dos_setvect(0x1c, clock_interrupt);

    set_clock_divisor(MY_DIVISOR);
}

void clock_shutdown()
{
    _dos_setvect(0x1c, old_clock_interrupt);
    set_clock_divisor(-1);
}
