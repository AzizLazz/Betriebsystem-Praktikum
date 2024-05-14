#ifndef EXCEPTION_HANDLING_H
#define EXCEPTION_HANDLING_H

#include<lib/ringbuffer.h>
extern int dump;

extern RingBuffer* buffer;
extern int unterprogram;

extern int kernelinterrupt;

extern int end ; 

void set_end(int ctrl);

void set_kernelinterrupt(int ctrl);
void set_unterprogram(int ctrl);



RingBuffer* get_buffer();


void dump_en(int ctrl);
int get_dump_value();

char* DFSR_Status();
char* IFSR_Status();

char* moduskodierung(unsigned int reg);
char* spsr_bits(unsigned int spsr, char* string);

void firstline_da(unsigned int addr);
void firstline_pa(unsigned int addr);
void firstline_spv(unsigned int addr);
void firstline_und(unsigned int addr);
void firstline_irq(unsigned int addr);

void data_abort_c(unsigned int* ptr,unsigned int spsr);
void Prefetch_abort_c(unsigned int* ptr,unsigned int spsr);
void supervisor_c(unsigned int* ptr,unsigned int spsr);
void undefined_c(unsigned int* ptr,unsigned int spsr);
void irq_extra_handler(unsigned int *ptr);
void irq_c(unsigned int* ptr,unsigned int spsr);


#endif  // EXCEPTION_HANDLING_H

