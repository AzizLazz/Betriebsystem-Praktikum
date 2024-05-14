#ifndef UART_H
#define UART_H
#include<lib/ringbuffer.h>

int is_busy();
int transmit_fifo_full () ;
int receive_fifo_empty ();
int uart_vorbi (int aufgabe) ;
void transmit( char t) ;
char recieve () ;
int deaktivate_fifo (int ctrl) ;
void interrupt (int ctrl) ;


#endif 
