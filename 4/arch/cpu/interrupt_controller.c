#include <stdint.h>
#include <config.h>
#include <kernel/kprintf.h>
#define sys_Base (0x7E00B200 - 0x3F000000) 
#define IRQ1 (1 << 1)
#define IRQ3 (1 << 3)
#define IRQUART (1 << 25)


struct interrupt_control {
  unsigned int irq_basic_pending ;
  unsigned int irq_pending_1 ;
  unsigned int irq_pending_2;
  unsigned int fiq_control ;
  unsigned int enable_irq_1 ;
  unsigned int enable_irq_2;
  unsigned int enable_basic_irq ;
  unsigned int disable_irq_1;
  unsigned int disable_irq_2;
  unsigned int disable_basic_irq;
} ;

volatile  struct interrupt_control*  new_interrupt_control = (struct interrupt_control *) sys_Base ;

void activate_irq_types(){
    new_interrupt_control->enable_irq_1 |= IRQ1;
    new_interrupt_control->enable_irq_1 |= IRQ3;    //für systimer 1 und 3
    new_interrupt_control->enable_irq_2 |= IRQUART; //für uart
}

int which_irq(){
    //für uart 
    if(((new_interrupt_control->irq_pending_2>>25) & 1)==1){  
        return 1;
    }
    //für systimer
    else if(((new_interrupt_control->irq_pending_1>>1 )& 1)==1 ){
        return 0;
    }
    else if (((new_interrupt_control->irq_pending_1>>3)& 1)==1)
    {
        return 3;
    }
    
    else {
        return -1;
    }
}
