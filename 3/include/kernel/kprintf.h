#ifndef KPRINTF_H
#define KPRINTF_H

#include <stdarg.h>

// Function to print the Error Messages
void print_uart_errors();

// Function to print a string to UART
void print_uart(char *format);

// Function to print an integer with formatting
void print_uart_standard(int zahl, int width, int zeros);
void print_uart_standardu(unsigned int zahl, int width, int zeros ) ;
// Function to print a hexadecimal string with '0x' prefix and formatting
void print_uart_p(unsigned long format, int width);

// Function to print a hexadecimal integer with formatting
void print_uart_x(unsigned int format, int width, int zeros);

// Function to check the size specifier in the format string
int check_size(char *format);

// Main printf-like function that supports formatting
void kprintf(char *eingabe, ...);


#endif
