#include<kernel/asm_hilfsfunktionen.h>
#include<kernel/kprintf.h>
#include <lib/helpers.h>
#include<arch/cpu/interrupt_controller.h>
#include<config.h>
#include<lib/ringbuffer.h>
#include<arch/bsp/uart.h>
#include<arch/cpu/sys_timer.h>

int dump = 0;
RingBuffer* buffer;

int unterprogram = 0 ; 

void set_unterprogram(int ctrl){
  unterprogram = ctrl ; 
}

RingBuffer* get_buffer(){
  return buffer;
}


void dump_en (int ctrl) {
  dump=ctrl ;
}
int get_dump_value(){
  return dump;
}

char* DFSR_Status() {
    unsigned int DFSR = _DFSR();                          

    unsigned int combinedBits = ((DFSR>>10 & 1) << 4) | ((DFSR>>3 & 1) << 3) | ((DFSR>>2 & 1) << 2) | ((DFSR>>1 & 1) << 1) | ((DFSR>>0)& 1);
    switch (combinedBits) {
        case 0b00000: return "No function, reset value";
        case 0b00001: return "Alignment fault";
        case 0b00010: return "Debug event fault";
        case 0b00011: return "Access Flag fault on Section";
        case 0b00100: return "Cache maintenance operation fault";
        case 0b00101: return "Translation fault on Section";
        case 0b00110: return "Access Flag fault on Page";
        case 0b00111: return "Translation fault on Page";
        case 0b01000: return "Precise External Abort";
        case 0b01001: return "Domain fault on Section";
        case 0b01011: return "Domain fault on Page";
        case 0b01100: return "External abort on Section";
        case 0b01101: return "Permission fault on Section";
        case 0b01110: return "External abort on Page";
        case 0b01111: return "Permission fault on Page";
        case 0b10110: return "Imprecise External Abort";
        default: return "No function";
    }
}

char* IFSR_Status() {
    unsigned int IFSR = _IFSR();
    unsigned int combinedBits = ((IFSR>>10 & 1) << 4) | ((IFSR>>3 & 1) << 3) | ((IFSR>>2 & 1) << 2) | ((IFSR>>1 & 1) << 1) | ((IFSR>>0)& 1);
    switch (combinedBits) {
        case 0b00000: return "No function, reset value";
        case 0b00010: return "Debug event fault";
        case 0b00011: return "Access Flag fault on Section";
        case 0b00101: return "Translation fault on Section";
        case 0b00110: return "Access Flag fault on Page";
        case 0b00111: return "Translation fault on Page";
        case 0b01000: return "Precise External Abort";
        case 0b01001: return "Domain fault on Section";
        case 0b01011: return "Domain fault on Page";
        case 0b01100: return "External abort on Section";
        case 0b01101: return "Permission fault on Section";
        case 0b01110: return "External abort on Page";
        case 0b01111: return "Permission fault on Page";
        default: return "No function";
    }
}

char* moduskodierung( unsigned int reg){
   unsigned int combinedBits = ((reg>>4 & 1) << 4) | ((reg>>3 & 1) << 3) | ((reg>>2 & 1) << 2) | ((reg>>1 & 1) << 1) | ((reg>>0)& 1);
    switch (combinedBits) {
        case 0b10000: return "User";
        case 0b10001: return "FIQ";
        case 0b10010: return "IRQ";
        case 0b10011: return "Supervisor";
        case 0b10111: return "Abort";
        case 0b11011: return "Undefined";
        case 0b11111: return "System";
        default: return "Invalid";
    }
}

char* spsr_bits(unsigned int spsr, char* string){ 
  for (int i = 31; i >= 0; i--) {
        if ((spsr >> i) & 1) {
            switch (i) {
                case 31: string[0] = 'N'; break;
                case 30: string[1] = 'Z'; break;
                case 29: string[2] = 'C'; break;
                case 28: string[3] = 'V'; break;
                case 9:  string[5] = 'E'; break;
                case 7:  string[7] = 'I'; break;
                case 6:  string[8] = 'F'; break;
                case 5:  string[9] = 'T'; break;
            }
        }
    }
    return string;
}

void firstline_da(unsigned int addr){
  kprintf("############  EXCEPTION  ############\n\n") ;
  kprintf("Data Abort an Adresse:0x%08x\n",addr) ; 
}
void firstline_pa(unsigned int addr){
  kprintf("############  EXCEPTION  ############\n\n") ;

  kprintf("Prefetch Abort an Adresse:0x%08x\n",addr) ;
}
void firstline_spv(unsigned int addr){
  kprintf("############  EXCEPTION  ############\n\n") ;

  kprintf("Supervisor Call an Adresse:0x%08x\n",addr) ;

}
void firstline_und(unsigned int addr){
  
  kprintf("############  EXCEPTION  ############\n\n") ;

  kprintf("Undefined an Adresse:0x%08x\n",addr) ;

}
void firstline_irq(unsigned int addr){
  if(dump == 1){
    kprintf("############  EXCEPTION  ############\n\n") ;

    kprintf("Irq Call an Adresse:0x%08x\n",addr) ;
}
}

void data_abort_c (unsigned int* ptr, unsigned int spsr) {
  char cpsr_status_bits[] = "____ _ ___";
  spsr_bits(cpsr(),cpsr_status_bits);

  char irq_status_bits[] = "____ _ ___";
  spsr_bits(spsr_irq(),irq_status_bits);

  char abt_status_bits[] = "____ _ ___";
  spsr_bits(spsr,abt_status_bits);
  
  char und_status_bits[] = "____ _ ___";
  spsr_bits(spsr_und(),und_status_bits);
  
  char svc_status_bits[] = "____ _ ___";
  spsr_bits(spsr_svc(),svc_status_bits);

  char* cpsr_modus = moduskodierung(cpsr());
  char* spsr_modus_irq = moduskodierung(spsr_irq());
  char* spsr_modus_abt = moduskodierung(spsr);
  char* spsr_modus_und = moduskodierung(spsr_und());
  char* spsr_modus_svc = moduskodierung(spsr_svc());

  char* dfsrstatus = DFSR_Status();

  kprintf("Data Fault Status Register:0x%08x -> %s\n",_DFSR(),dfsrstatus) ;
  kprintf("Data Fault Adress Register:0x%08x\n\n",_DFAR()) ;

  kprintf(">> Registerschnapschuss <<\n") ;
  kprintf("R0: 0x%08x R5: 0x%08x R10: 0x%08x\n",ptr[0],ptr[5],ptr[10]) ;
  kprintf("R1: 0x%08x R6: 0x%08x R11: 0x%08x\n",ptr[1],ptr[6],ptr[11]) ;
  kprintf("R2: 0x%08x R7: 0x%08x R12: 0x%08x\n",ptr[2],ptr[7],ptr[12]) ;
  kprintf("R3: 0x%08x R8: 0x%08x\n",ptr[3],ptr[8]) ;
  kprintf("R4: 0x%08x R9: 0x%08x\n\n",ptr[4],ptr[9]) ;

  kprintf(">> Modusspezifische Register <<\n\n") ;

  kprintf("User/System | LR:0x%08x  | SP:0x%08x  | CPSR:%s %s 0x%08x\n", lr_usr(), sp_usr(), cpsr_status_bits,cpsr_modus,cpsr());
  kprintf("IRQ         | LR:0x%08x  | SP:0x%08x  | SPSR:%s %s 0x%08x\n", lr_irq(), sp_irq(), irq_status_bits,spsr_modus_irq,spsr_irq());
 
  kprintf("Abort       | LR:0x%08x  | SP:0x%08x  | SPSR:%s %s 0x%08x\n", lr(), sp(), abt_status_bits,spsr_modus_abt ,spsr);
 
  kprintf("Undefined   | LR:0x%08x  | SP:0x%08x  | SPSR:%s %s 0x%08x\n", lr_und(), sp_und(), und_status_bits, spsr_modus_und,spsr_und());
  kprintf("Supervisor  | LR:0x%08x  | SP:0x%08x  | SPSR:%s %s 0x%08x\n", lr_svc(), sp_svc(), svc_status_bits, spsr_modus_svc,spsr_svc());
  while(1){}

}

void Prefetch_abort_c (unsigned int* ptr,unsigned int spsr) {
  char cpsr_status_bits[] = "____ _ ___";
  spsr_bits(cpsr(),cpsr_status_bits);

  char irq_status_bits[] = "____ _ ___";
  spsr_bits(spsr_irq(),irq_status_bits);

  char abt_status_bits[] = "____ _ ___";
  spsr_bits(spsr,abt_status_bits);
  
  char und_status_bits[] = "____ _ ___";
  spsr_bits(spsr_und(),und_status_bits);
  
  char svc_status_bits[] = "____ _ ___";
  spsr_bits(spsr_svc(),svc_status_bits);

  char* cpsr_modus = moduskodierung(cpsr());
  char* spsr_modus_irq = moduskodierung(spsr_irq());
  char* spsr_modus_abt = moduskodierung(spsr);
  char* spsr_modus_und = moduskodierung(spsr_und());
  char* spsr_modus_svc = moduskodierung(spsr_svc());
  
  char* ifsrstatus = IFSR_Status();

  kprintf("Instruktion Fault Status Register:0x%08x -> %s\n",_IFSR(),ifsrstatus) ;
  kprintf("Instruktion Fault Adress Register:0x%08x\n\n",_IFAR()) ;

  kprintf(">> Registerschnapschuss <<\n") ;
  kprintf("R0: 0x%08x R5: 0x%08x R10: 0x%08x\n",ptr[0],ptr[5],ptr[10]) ;
  kprintf("R1: 0x%08x R6: 0x%08x R11: 0x%08x\n",ptr[1],ptr[6],ptr[11]) ;
  kprintf("R2: 0x%08x R7: 0x%08x R12: 0x%08x\n",ptr[2],ptr[7],ptr[12]) ;
  kprintf("R3: 0x%08x R8: 0x%08x\n",ptr[3],ptr[8]) ;
  kprintf("R4: 0x%08x R9: 0x%08x\n\n",ptr[4],ptr[9]) ;

  kprintf(">> Modusspezifische Register <<\n\n") ;

  kprintf("User/System | LR:0x%08x  | SP:0x%08x  | CPSR:%s %s 0x%08x\n", lr_usr(), sp_usr(), cpsr_status_bits,cpsr_modus,cpsr());
  kprintf("IRQ         | LR:0x%08x  | SP:0x%08x  | SPSR:%s %s 0x%08x\n", lr_irq(), sp_irq(), irq_status_bits, spsr_modus_irq,spsr_irq());
  kprintf("Abort       | LR:0x%08x  | SP:0x%08x  | SPSR:%s %s 0x%08x\n", lr(), sp(), abt_status_bits, spsr_modus_abt ,spsr);
  kprintf("Undefined   | LR:0x%08x  | SP:0x%08x  | SPSR:%s %s 0x%08x\n", lr_und(), sp_und(), und_status_bits, spsr_modus_und ,spsr_und());
  kprintf("Supervisor  | LR:0x%08x  | SP:0x%08x  | SPSR:%s %s 0x%08x\n", lr_svc(), sp_svc(), svc_status_bits,  spsr_modus_svc,spsr_svc());
  while(1){}
}

void supervisor_c (unsigned int* ptr,unsigned int spsr){
  char cpsr_status_bits[] = "____ _ ___";
  spsr_bits(cpsr(),cpsr_status_bits);

  char irq_status_bits[] = "____ _ ___";
  spsr_bits(spsr_irq(),irq_status_bits);

  char abt_status_bits[] = "____ _ ___";
  spsr_bits(spsr_abt(),abt_status_bits);
  
  char und_status_bits[] = "____ _ ___";
  spsr_bits(spsr_und(),und_status_bits);
  
  char svc_status_bits[] = "____ _ ___";
  spsr_bits(spsr,svc_status_bits);

  char* cpsr_modus = moduskodierung(cpsr());
  char* spsr_modus_irq = moduskodierung(spsr_irq());
  char* spsr_modus_abt = moduskodierung(spsr_abt());
  char* spsr_modus_und = moduskodierung(spsr_und());
  char* spsr_modus_svc = moduskodierung(spsr);

  kprintf(">> Registerschnapschuss <<\n") ;
  kprintf("R0: 0x%08x R5: 0x%08x R10: 0x%08x\n",ptr[0],ptr[5],ptr[10]) ;
  kprintf("R1: 0x%08x R6: 0x%08x R11: 0x%08x\n",ptr[1],ptr[6],ptr[11]) ;
  kprintf("R2: 0x%08x R7: 0x%08x R12: 0x%08x\n",ptr[2],ptr[7],ptr[12]) ;
  kprintf("R3: 0x%08x R8: 0x%08x\n",ptr[3],ptr[8]) ;
  kprintf("R4: 0x%08x R9: 0x%08x\n\n",ptr[4],ptr[9]) ;

  kprintf(">> Modusspezifische Register <<\n\n") ;

  kprintf("User/System | LR:0x%08x  | SP:0x%08x  | CPSR:%s %s 0x%08x\n", lr_usr(), sp_usr(), cpsr_status_bits,cpsr_modus,cpsr());
  kprintf("IRQ         | LR:0x%08x  | SP:0x%08x  | SPSR:%s %s 0x%08x\n", lr_irq(), sp_irq(), irq_status_bits, spsr_modus_irq,spsr_irq());
  kprintf("Abort       | LR:0x%08x  | SP:0x%08x  | SPSR:%s %s 0x%08x\n", lr_abt(), sp_abt(), abt_status_bits,  spsr_modus_abt,spsr_abt());
  kprintf("Undefined   | LR:0x%08x  | SP:0x%08x  | SPSR:%s %s 0x%08x\n", lr_und(), sp_und(), und_status_bits, spsr_modus_und ,spsr_und());
  kprintf("Supervisor  | LR:0x%08x  | SP:0x%08x  | SPSR:%s %s 0x%08x\n", lr(), sp(), svc_status_bits, spsr_modus_svc ,spsr);
  while(1){}
}

void undefined_c (unsigned int* ptr, unsigned int spsr) {
  char cpsr_status_bits[] = "____ _ ___";
  spsr_bits(cpsr(),cpsr_status_bits);

  char irq_status_bits[] = "____ _ ___";
  spsr_bits(spsr_irq(),irq_status_bits);

  char abt_status_bits[] = "____ _ ___";
  spsr_bits(spsr_abt(),abt_status_bits);
  
  char und_status_bits[] = "____ _ ___";
  spsr_bits(spsr,und_status_bits);
  
  char svc_status_bits[] = "____ _ ___";
  spsr_bits(spsr_svc(),svc_status_bits);

  char* cpsr_modus = moduskodierung(cpsr());
  char* spsr_modus_irq = moduskodierung(spsr_irq());
  char* spsr_modus_abt = moduskodierung(spsr_abt());
  char* spsr_modus_und = moduskodierung(spsr);
  char* spsr_modus_svc = moduskodierung(spsr_svc());

  kprintf(">> Registerschnapschuss <<\n") ;
  kprintf("R0: 0x%08x R5: 0x%08x R10: 0x%08x\n",ptr[0],ptr[5],ptr[10]) ;
  kprintf("R1: 0x%08x R6: 0x%08x R11: 0x%08x\n",ptr[1],ptr[6],ptr[11]) ;
  kprintf("R2: 0x%08x R7: 0x%08x R12: 0x%08x\n",ptr[2],ptr[7],ptr[12]) ;
  kprintf("R3: 0x%08x R8: 0x%08x\n",ptr[3],ptr[8]) ;
  kprintf("R4: 0x%08x R9: 0x%08x\n\n",ptr[4],ptr[9]) ;

  kprintf(">> Modusspezifische Register <<\n\n") ;

  kprintf("User/System | LR:0x%08x  | SP:0x%08x  | CPSR:%s %s 0x%08x\n", lr_usr(), sp_usr(), cpsr_status_bits,cpsr_modus,cpsr());
  kprintf("IRQ         | LR:0x%08x  | SP:0x%08x  | SPSR:%s %s 0x%08x\n", lr_irq(), sp_irq(), irq_status_bits,spsr_modus_irq ,spsr_irq());
  kprintf("Abort       | LR:0x%08x  | SP:0x%08x  | SPSR:%s %s 0x%08x\n", lr_abt(), sp_abt(), abt_status_bits, spsr_modus_abt ,spsr_abt());
  kprintf("Undefined   | LR:0x%08x  | SP:0x%08x  | SPSR:%s %s 0x%08x\n", lr(), sp(), und_status_bits, spsr_modus_und ,spsr);
  kprintf("Supervisor  | LR:0x%08x  | SP:0x%08x  | SPSR:%s %s 0x%08x\n", lr_svc(), sp_svc(), svc_status_bits, spsr_modus_svc ,spsr_svc());
  while(1){} 
}

void irq_extra_handler(){
  int x = which_irq();
  
  if (x == 1) //uart
  {
    
    char new_char = recieve() ;
    enqueue(new_char); 
  }
  else if (x == 0) //sysstimer
  {
    increase_compare_value();
    if (unterprogram){
      kprintf("!\n");
    }
    reset_interrupt();
  }
  else{ 
    kprintf("Es ist ein Fehler beim IRQ handling passiert");
  }
  
  
}

void irq_c (unsigned int* ptr,unsigned int spsr) {
  if(dump == 1){
    char cpsr_status_bits[] = "____ _ ___";
    spsr_bits(cpsr(),cpsr_status_bits);

    char irq_status_bits[] = "____ _ ___";
    spsr_bits(spsr,irq_status_bits);

    char abt_status_bits[] = "____ _ ___";
    spsr_bits(spsr_abt(),abt_status_bits);
    
    char und_status_bits[] = "____ _ ___";
    spsr_bits(spsr_und(),und_status_bits);
    
    char svc_status_bits[] = "____ _ ___";
    spsr_bits(spsr_svc(),svc_status_bits);

      char* cpsr_modus = moduskodierung(cpsr());
      char* spsr_modus_irq = moduskodierung(spsr);
      char* spsr_modus_abt = moduskodierung(spsr_abt());
      char* spsr_modus_und = moduskodierung(spsr_und());
      char* spsr_modus_svc = moduskodierung(spsr_svc());

      kprintf(">> Registerschnapschuss <<\n") ;
      kprintf("R0: 0x%08x R5: 0x%08x R10: 0x%08x\n",ptr[0],ptr[5],ptr[10]) ;
      kprintf("R1: 0x%08x R6: 0x%08x R11: 0x%08x\n",ptr[1],ptr[6],ptr[11]) ;
      kprintf("R2: 0x%08x R7: 0x%08x R12: 0x%08x\n",ptr[2],ptr[7],ptr[12]) ;
      kprintf("R3: 0x%08x R8: 0x%08x\n",ptr[3],ptr[8]) ;
      kprintf("R4: 0x%08x R9: 0x%08x\n",ptr[4],ptr[9]) ;

      kprintf(">> Modusspezifische Register <<\n\n") ;

      kprintf("User/System | LR:0x%08x  | SP:0x%08x  | CPSR:%s %s 0x%08x\n", lr_usr(), sp_usr(), cpsr_status_bits,cpsr_modus,cpsr());
      kprintf("IRQ         | LR:0x%08x  | SP:0x%08x  | SPSR:%s %s 0x%08x\n", lr(), sp(), irq_status_bits, spsr_modus_irq,spsr);
      kprintf("Abort       | LR:0x%08x  | SP:0x%08x  | SPSR:%s %s 0x%08x\n", lr_abt(), sp_abt(), abt_status_bits, spsr_modus_abt ,spsr_abt());
      kprintf("Undefined   | LR:0x%08x  | SP:0x%08x  | SPSR:%s %s 0x%08x\n", lr_und(), sp_und(), und_status_bits, spsr_modus_und ,spsr_und());
      kprintf("Supervisor  | LR:0x%08x  | SP:0x%08x  | SPSR:%s %s 0x%08x\n", lr_svc(), sp_svc(), svc_status_bits, spsr_modus_svc ,spsr_svc());
    }
}

