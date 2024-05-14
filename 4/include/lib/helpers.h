#ifndef HELPERS_H
#define HELPERS_H

#include <stdarg.h>


int strlength(char *format);

char* intToString(int num, char *str, int base, int isSigned);
char* intToStringu(unsigned int num, char *str, int base) ;

char* intToHex(unsigned long dez, char* result); 
int* dezimalZuBinär(int dezimal,int* result, int anzahlStellen) ;

#endif
