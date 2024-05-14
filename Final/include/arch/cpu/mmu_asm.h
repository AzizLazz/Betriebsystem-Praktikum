#ifndef _MMU_ASM_H_
#define _MMU_ASM_H_

unsigned int read_TTBR0();
void set_TTBR0(unsigned int x);                 

unsigned int read_DACR();
void set_DACR()  ;                   

void enable_SCTLR();

void enable_32bit_TTBCR();

void disable_caches();
void tlbhandling();

#endif
