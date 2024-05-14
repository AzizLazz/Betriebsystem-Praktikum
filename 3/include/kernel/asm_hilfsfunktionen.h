#ifndef ASM_HILFSFUNKTIONEN_H
#define ASM_HILFSFUNKTIONEN_H

unsigned int lr();
unsigned int cpsr();
unsigned int sp();
unsigned int spsr();

unsigned int _IFSR();
unsigned int _IFAR();
unsigned int _DFAR();
unsigned int _DFSR();

unsigned int sp_usr();
unsigned int lr_usr();

unsigned int sp_fiq();
unsigned int lr_fiq();
unsigned int spsr_fiq();

unsigned int sp_irq();
unsigned int lr_irq();
unsigned int spsr_irq();

unsigned int lr_svc();
unsigned int sp_svc();
unsigned int spsr_svc();

unsigned int lr_abt();
unsigned int sp_abt();
unsigned int spsr_abt();

unsigned int lr_und();
unsigned int sp_und();
unsigned int spsr_und();

#endif
