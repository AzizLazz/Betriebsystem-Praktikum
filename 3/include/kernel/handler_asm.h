#ifndef INTERRUPT_HANDLERS_H
#define INTERRUPT_HANDLERS_H


void _init();
void do_abort();
void do_und();
void set_registers(unsigned int x, unsigned int y,unsigned int z);


#endif  
