#ifndef sys_timer_H
#define sys_timer_H
#include <config.h>


void set_compare_value(int channel, int Wert);

void reset_compare_value(int channel) ;

void increase_compare_value();
void reset_interrupt();

#endif
