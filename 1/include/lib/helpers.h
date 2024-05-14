#ifndef HELPERS_H
#define HELPERS_H

#include <stdarg.h>


int strlength(char *format);

char* intToString(int num, char *str, int base, int isSigned);

char* intToHex(unsigned long dez, char* result); 


#endif
