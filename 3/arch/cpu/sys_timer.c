#include <stdint.h>
#include <config.h>
#include <kernel/kprintf.h>
#define sys_Base (0x7E003000 - 0x3F000000)
#define CS_M1 (1<<1)

struct systimer {
  unsigned int cs ;
  unsigned int clo ;
  unsigned int chi ;
  unsigned int c0 ;
  unsigned int c1 ;
  unsigned int c2 ;
  unsigned int c3 ;
} ;

volatile  struct systimer *  new_sys_timer = (struct systimer *) sys_Base ;


void set_compare_value(int channel, int Wert){				
   switch (channel){

        case 1 :
        	new_sys_timer->c1 = Wert ;
        	break ;
        
        case 3 :
        	new_sys_timer->c3 = Wert ;
        	break ;
        default :
        	break ;
      }
}

void reset_compare_value(int channel) {
  switch (channel){

        case 1 :
        	new_sys_timer->c1 = 0 ;
        	break ;

        case 3 :
        	new_sys_timer->c3 = 0 ;
        	break ;
        default :
        	break ;
      }
}

void increase_compare_value(){
	new_sys_timer->c1 += TIMER_INTERVAL ;
}

void reset_interrupt(){
	new_sys_timer->cs |= CS_M1;
}


