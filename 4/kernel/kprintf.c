#include "stdarg.h"
#include <arch/bsp/uart.h>
#include <lib/helpers.h>

void print_uart_errors() {
    char err[] = "Error: Es gab eine Fehlerhafte Eingabe\n" ; 
    int len = strlength(err) ;
    int i = 0 ; 

    while (i < len) {
        transmit(err[i]);
        i++;
    }
}

void print_uart(char* format) {
    while (*format != '\0') {
        transmit(*format);
        format++;
    }
}

void print_uart_standard(int zahl, int width, int zeros ) {
    
        char temp2[20];
        char* format = intToString(zahl, temp2, 10, 1);
        int i = 0;
        int len = strlength(format);
        int temp = width;
        
        if (format[i] == '-' && zeros == 1) {
            transmit('-');
            i++;
        }
        while (temp > len) {
            if (zeros == 1) {
                transmit('0');
            temp--;
            } 
            else {
                transmit(' ');
                temp--;
            }
        }
        
        if (format[i] == '-') {
            transmit('-');
            i++;
        }

        while (i < len) {
            transmit(format[i]);
            i++;
        }
    
}

void print_uart_standardu(unsigned int zahl, int width, int zeros ) {
    
        char temp2[20];
        char* format = intToStringu(zahl, temp2, 10);
        int i = 0;
        int len = strlength(format);
        int temp = width;
  
        while (temp > len) {
            if (zeros == 1) {
                transmit('0');
            temp--;
            } 
            else {
                transmit(' ');
                temp--;
            }
        }
        
 
        while (i < len) {
            transmit(format[i]);
            i++;
        }
    
}

void print_uart_p(unsigned long format, int width) {
    int i = 0;
    int temp = width;
    char temp2[20];
    char* format_hexa = intToHex(format,temp2);
    int len = strlength(format_hexa);

    if (temp>=8){
        while (temp-2 > len) {
            transmit(' ');
            temp--;
        }
    }

    transmit('0');
    transmit('x');
        
    while (i < len) {
        transmit(format_hexa[i]);
        i++;
    }
}

int check_size(char* format) {
    if ( *format == '8') {
        return 1 ;
    } 
    else if ( *format == '0') {
        return 2 ;
    }
    else {
        return 0 ;
    }
}

void print_uart_x(unsigned int format, int width, int zeros) {
    int i = 0;
    int temp = width;
    char temp2[20];
    char *format_hexa = intToHex(format,temp2);
    int len = strlength(format_hexa);

    while (temp > len) {
        if (zeros == 1 ) {
            transmit('0');
            temp--;
        } 
        else {
            transmit(' ');
            temp--;
        }
    }
    

    while (i < len) {
        transmit(format_hexa[i]);
        i++;
    }
}

void kprintf(char* eingabe, ...) {
    int test = 1;
    char *temp = eingabe;
    while (*temp != '\0') { //Fehlerüberprüfung
        if (*temp == '%') {
            temp++;
            if (*temp == '%' || *temp == 'i' || *temp == 'c' || *temp == 'x' || *temp == 'u' || *temp == 'p' || *temp == 's') {
                temp++;
            } 
            else if (*temp == '8') {
                temp++;
                if (*temp == 'i' || *temp == 'u' || *temp == 'x' || *temp == 'p') {
                    temp++;
                } else {
                    test = 0;
                    print_uart_errors();
                    break;
                }
            }
            else if (*temp == '0') {
                temp++;
                if (*temp == '8') {
                    temp++;
                    if (*temp == 'i' || *temp == 'u' || *temp == 'x') {
                        temp++;
                    } 
                    else {
                        test = 0;
                        print_uart_errors();
                        break;
                    }
                }
                else {
                    test = 0;
                    print_uart_errors();
                    break;
                }
                
                
            }  
            else {
                test = 0;
                print_uart_errors();
                break;
            }
        } 
        else{
            temp++;
        }
    }

    if (test){
        va_list args;
        va_start(args, eingabe);
        int width = 0;
        int zeros = 0;
        int negativ = 0;

        while (*eingabe != '\0') {
            if (*eingabe != '%') {
                transmit(*eingabe);
            }

            else {
                eingabe++; 
                if (check_size(eingabe) == 2) {
                    zeros = 1;
                    eingabe++;
                }
                if (check_size(eingabe) == 1) {
                    width = *eingabe - '0';
                    eingabe++;

                }

                switch (*eingabe) {
                    case 'c':
                        transmit(va_arg(args, int)); 
                        break;

                    case 's':
                        print_uart(va_arg(args, char *)); 
                        break;

                    case 'x':
                        print_uart_x(va_arg(args, unsigned int), width, zeros); 
                        width = 0;
                        zeros = 0;
                        break;

                    case 'i':
                        print_uart_standard(va_arg(args, int), width, zeros); 
                        width = 0;
                        zeros = 0;
                        break;

                    case 'u':
                       negativ = va_arg(args,unsigned int);
                            print_uart_standardu(negativ, width, zeros); 
                            width = 0;
                            zeros = 0;
                            break ;
              
                    case 'p':
                        print_uart_p((unsigned long) va_arg(args, void *), width);
                        width = 0;
                        break;

                    case '%':
                        transmit('%');
                        break;

                    default:
                        print_uart_errors();
                        break;
                }
            }
            eingabe++;
        }
        va_end(args);
    }  
}
