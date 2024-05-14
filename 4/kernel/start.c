
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
#include <lib/ringbuffer.h>
#include <kernel/asm_hilfsfunktionen.h>
#include <lib/helpers.h>
#include <kernel/handler_asm.h>
#include <arch/cpu/interrupt_controller.h>
#include <kernel/scheduler.h>

volatile unsigned int counter = 0;


void increment_counter() {
	counter++;
}

void start_kernel(){

	_init();								//setzt vbar und initialisiert stacks
	activate_irq_types(); 					//enable irq interrupts 
	set_compare_value(1,TIMER_INTERVAL); 	
	initglobal_buffer();
	set_unterprogram(1);
	interrupt(1); 				//enabled die uart interrupts
	uart_vorbi(2) ;
	init_idlethread();						//initialisiert den idle thread
	init_mainthread();						//initialisiert den main thread
	kprintf("=== Betriebssystem gestartet ===\n");
	test_kernel(); 

	//erklärung des Programms, ich habs nicht hinbekommen den usercode und kernel code zu isolieren
	//in main.c steht die aufrufkonvention unserer systemrufe (die "Bilbliothek")
	//nach einem Tick der Zeitscheibe wird der main-Thread geladen und kann dann Zeichen empfangen

	
	// Endless counter
	for (;;) {
		increment_counter();
	}

}
