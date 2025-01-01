#ifndef _SYS_H
#define _SYS_H

void timer_wait();
void timer_custom_wait(unsigned long ticks);

void clock_init();
void clock_shutdown();

int is_key_pressed();

char get_key_code();

#endif // _SYS_H
