#ifndef _CLOCK_H
#define _CLOCK_H

void clock_init();
void clock_shutdown();

void timer_wait();
void timer_custom_wait(unsigned long);

#endif // _CLOCK_H
