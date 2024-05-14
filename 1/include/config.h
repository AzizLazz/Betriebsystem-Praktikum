#ifndef _CONFIG_H_
#define _CONFIG_H_

/* Falls auf den Boards gearbeitet wird am besten
 * die nächste Zeile auskommentieren
 */
#define __QEMU__

/**
 * \file config.h
 *
 * Enthält defines und static Funktionen zum testen der
 * Implementierung. Wir tauschen diese Werte/Funktionen beim
 * Korrigieren zum Testen aus. Bitte fügt hier KEINE weiteren
 * defines oÄ ein. Ihr könnt diese Werte zum Testen natürlich
 * auch gerne selbst verändern.
 */

/* Include-Pfad bitte gegenfalls anpassen */
#include <kernel/kprintf.h>
static inline void test_kprintf(void) {
	/* Nur für Aufgabenblatt 1
	 * Hier Test Funktion für kprintf schreiben und
	 * nach dem vollständigen initialisieren aufrufen
	 */
    int   my_var = 1234 ;

    void * ptr = &my_var;
    char * s = "try_with_long_string" ;
	kprintf("kprintf_test\n");
	kprintf("i:%i\n", my_var);	
    kprintf("-i:%i\n", -my_var);	
    kprintf("8i:%8i\n", my_var);
    kprintf("8-i:%8i\n", -my_var);
    kprintf("08i:%08i\n", my_var);
    kprintf("08-i:%08i\n", -my_var);
    
    kprintf("x:%x\n", my_var);	
    kprintf("8x:%8x\n", my_var);
    kprintf("08x:%08x\n", my_var);
    

    kprintf("u:%u\n", my_var);	
    kprintf("-u:%u\n", -my_var);	
    kprintf("8iu:%8u\n", my_var);
    kprintf("8-u:%8u\n", -my_var);
    kprintf("08u:%08u\n", my_var);
    kprintf("08-u:%08u\n", -my_var);

    kprintf("p:%p\n", (void *)ptr);	
    kprintf("8p:%8p\n", (void *)ptr);
    
    kprintf("s:%s\n", s);
    
    kprintf("var:%%\n");
    
    kprintf("var:%z\n", my_var);
    kprintf("var:%z\n", my_var);
    kprintf("var:%08p\n", (void *)ptr);
    kprintf("var:%07p\n", (void *)ptr);
    kprintf("var:%07i\n", (void *)ptr);
    kprintf("var:%18p\n", (void *)ptr);
    kprintf("var:%1s\n", (void *)ptr);
    kprintf("var:%8s\n", (void *)ptr);
    kprintf("08i:%08i, 8i:%8i, i:%i, 08i:%08i, 8i:%8i,i:%i\n" ,my_var,my_var,my_var,-my_var,-my_var,-my_var);
    kprintf("08x:%08x, 8x:%8x, x:%x, 08x:%08x, 8x:%8x\n" ,my_var,my_var,my_var,my_var,my_var);
}

/**
 * Erst ab Aufgabenblatt 2 relevant
 */

#ifdef __QEMU__
/* Werte zum testen unter QEMU */
#define BUSY_WAIT_COUNTER 10000000
#else
/* Werte zum testen auf der Hardware */
#define BUSY_WAIT_COUNTER 30000
#endif // __QEMU__

// Wir testen nur mit Werten die durch 2^n darstellbar sind
#define UART_INPUT_BUFFER_SIZE 128

// Timer Interrupt Interval zum testen in Mikrosekunden
// Systimer taktet mit 1MHz
// 1000000 -> 1 Sekunde
#define TIMER_INTERVAL 1000000


#endif // _CONFIG_H_
