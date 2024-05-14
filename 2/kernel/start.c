
/// Das vorherige Aufgabenblatt wurde als Demonstration wie die
/// binäre Lösung verwendet werden kann implementiert.
/// Die Implementierung darf/soll/muss abgeändert werden um das
/// nächste Aufgabenblatt zu implementieren.
/// Um die Demonstration nicht über zu viele Dateien zu verstreuen
/// ist alles in:
/// - kernel/start.c
/// - arch/cpu/entry.S
/// zu finden.
/// Es bietet sich jedoch an manche Funktionalitäten in andere
/// Dateien aus zu lagern.

//#include <arch/bsp/yellow_led.h>
#include <arch/bsp/uart.h>
#include <kernel/kprintf.h>
#include <config.h>
#include <arch/cpu/sys_timer.h>
#include <kernel/handler.h>
#include <lib/regcheck.h>
#include <lib/ringbuffer.h>
#include <kernel/asm_hilfsfunktionen.h>
#include <lib/helpers.h>
#include <kernel/handler_asm.h>
#include <arch/cpu/interrupt_controller.h>

volatile unsigned int counter = 0;


void increment_counter() {
	counter++;
}

void start_kernel(){
	_init();								//setzt vbar und initialisiert stacks
	activate_irq_types(); 					//enable irq interrupts 
	set_compare_value(1,TIMER_INTERVAL); 	
	initglobal_buffer();

	kprintf("=== Betriebssystem gestartet ===\n");
	//test_kernel();						//in config.h ist die deklaration auskommentiert, weil sonst Fehler entstehen

	while(1){
  	char x = recieve();
  	switch (x){
        	case 's' :
				__asm__("SVC #0") ; 		//Einen  Supervisor  Call  auslösen.
				break;
        	case 'a' :
				do_abort(); 				//Einen  Data  Abort  erzeugen.
        		break;
        	case 'p' :
				__asm__("BKPT") ; 			//Einen  Prefetch  Abort  erzeugen.
				break;
        	case 'u' :
				do_und();  					//Eine  Undefined  Instruction  ausführen.
        		break;
        	case 'c' :
				set_unterprogram(1) ;		//enabled das Printen von '!'
				register_checker();			//Den  Register  Checker  ausführen.
        		break;
        	case 'e' :
				set_unterprogram(1);
				interrupt(1); 				//enabled die uart interrupts
				uart_vorbi(2) ;
				 
				while(1){
					
					if (!isBufferEmpty())
					{
						char b = dequeue(get_buffer());
						for(int i = 0;i<10;i++){
							kprintf("%c",b) ;
		  					for(int w = 0;w<BUSY_WAIT_COUNTER;w++){asm("");}  	//kurzes Warten nach jedem Zeichen
						}
					
					}
				}
        		break;	
        	case 'd' :
				dump_en(!get_dump_value()) ;
        		break;
        	default :
        		break ;
      }
	}
	// Endless counter
	for (;;) {
		increment_counter();
	}

}
