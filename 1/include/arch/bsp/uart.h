#ifndef UART_H
#define UART_H

int is_busy();
int transmit_fifo_full () ;
int receive_fifo_empty ();
int uart_vorbi (int aufgabe) ;
void transmit( char t) ;
char recieve () ;



#endif 
