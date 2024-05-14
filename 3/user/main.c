#include<arch/bsp/uart.h>
#include<kernel/kprintf.h>
#include <kernel/handler_asm.h>
#include<config.h>
#include <stdlib.h>
#include<kernel/handler.h>
#include<lib/regcheck.h>

void main(void* args){
    set_end(1);
   char c = *((char*)args);
   switch(c){
        case'a':
            do_abort();   
            __asm__("SVC #0") ;               //Einen  Data  Abort  erzeugen.
            return;
        case'p':
            __asm__("BKPT") ; 
            __asm__("SVC #0") ;           //Einen  Prefetch  Abort  erzeugen.
            return;
        case'u':
            do_und();      
            __asm__("SVC #0") ;              //Eine  Undefined  Instruction  ausführen.
            return;
        case's':
            __asm__("SVC #0") ; 		//Einen  Supervisor  Call  auslösen. Thread beenden im handler?
            return;
        case'c':
            register_checker();
            __asm__("SVC #0") ; 
            return;

    }
    for(unsigned int n=0;n<5;n++){
        for(unsigned int i = 0   ;i<BUSY_WAIT_COUNTER;i++){asm("");}
            kprintf("%c",c);
    }
    __asm__("SVC #0") ;     //exit()

}

