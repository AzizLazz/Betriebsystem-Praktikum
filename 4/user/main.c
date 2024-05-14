#include<arch/bsp/uart.h>
#include<kernel/kprintf.h>
#include <kernel/handler_asm.h>
#include<config.h>
#include <stdlib.h>
#include<kernel/handler.h>
#include<kernel/asm_hilfsfunktionen.h>
#include<kernel/scheduler.h>
#include<lib/ringbuffer.h>

void busy_wait_loop(){
    //for(unsigned int i = 0   ;i<BUSY_WAIT_COUNTER;i++){asm("");} 
    for(unsigned int i = 0   ;i<100000000;i++){asm("");}  //würde das Verhalten besser verdeutlichen

}
/*
Aufrufkonvention für unserer Systemrufe:
- Aufrufen des gewünschten Systemcalls entsprechend dem namen
syscall_undef(1) - entspricht einem unbekannten systemaufruf
syscall_undef(2) - entspricht einem systemcall aus dem Kernel

syscall_transmit(char c) - erhält einen char und gibt ihn über die UART Schnittstelle aus 

syscall_recieve() - gibt das Zeichen zurück welches über die UART Schnittstelle aus eigegeben wurde

syscall_sleep(unsigned int time) - hält den aktuellen thread um *time* Mikrosekunden an

syscall_createthread(void(* func)(void *),const void *args,unsigned int arg_size)
- erstellt einen neuen thread mit einer Funktion für den Einsprung, einem Speicherbereich und dessen Größe

syscall_endthread() - beendet den aktuellen thread
*/



void syscall_undef(int nr){
     if (nr == 1)
        {
            set_svc_undef(1);
        __asm__("SVC #0") ;
        }
        else if (nr == 2)
        {
            set_kernelinterrupt(1);
            set_svc_undef(1);
            __asm__("SVC #0") ;
        }
        else{
            kprintf("ungültiger systemcall\n");
        }

    svc_undef = 1;
    __asm__("SVC #0") ;
}

void syscall_transmit(char c){ 
    svc_tra = 1;
    setr((unsigned int)c);
    __asm__("SVC #0") ;
}
char syscall_recieve(){
    set_svc_recv(1);
    __asm__("SVC #0") ;
    char x = (char)getr();
    return x;

}

void syscall_sleep(unsigned int time){ 
    set_svc_sleep(1);
    setr(time);
    __asm__("SVC #0") ;
}

void syscall_createthread(void(* func)(void *),const void *args,unsigned int arg_size){
    svc_createthread = 1;      
    setforthreadcreat(func, args,arg_size);
    __asm__("SVC #0") ;
}

void syscall_endthread(){
    svc_endthread= 1;
    __asm__("SVC #0") ;
}

void worker_thread(void *arg){
    char c = *((char *)arg) ; 
    if (c =='s'){
        syscall_undef(1);
    }
    else if (c == 'S')
    {
        syscall_undef(2);
    }

    else{
    for ( unsigned int i = 0 ;  i<10 ;i++ )  {
        syscall_transmit(c);
        if  ( c >= 'A' && c <= 'Z' )  {
            busy_wait_loop() ;
        } 
        else {
            syscall_sleep(500000); //entspricht einer halben Sekunde Schlafen
        }
    }
    }
    
    syscall_endthread();
}

void main(){
    
    for (;;) {

        char c = syscall_recieve();

        syscall_createthread(worker_thread , &c , sizeof(c)) ;
    }

}

