#include <user/syscalls.h>
#include <config.h>
#include <stddef.h>

char *exceptionstack = (char *) ( 128 * 1024 * 1024 - (1 * 4* 1024)); //adresse die 1KB im ersten exception stack ist
char *uart_addr = (char *) (0x7E201000 - 0x3F000000);  
char *systimer_addr = (char *) (0x7E003000 - 0x3F000000);
char *interruptcontroller_addr = (char *) (0x7E00B200 - 0x3F000000);

void __attribute__ ((noinline)) stackoverflow(char c) { //orientiert an Vorgabe 
	if ( c > '9'){
		 c = '1';
	}
	volatile char temp = c;
	syscall_putc(c);
	stackoverflow(temp + 1);
	syscall_putc(c);
}

void user_prog(void * arg) {
	char c = *((char *) arg);
	test_user(c);
	volatile char x = 'c';
	switch(c) {
		case 'n': 
		    // cppcheck-suppress nullPointer
			x = *((char *) NULL); 									// lesender  Zugriff  auf  Null-Pointer.
			break;
		case 'p':
		 	((void (*)(void))NULL)(); 								// Sprung  auf  Null-Pointer
		 	break; 
		case 'd':
		 	x = *((unsigned int *)((unsigned int)1048576*3)); 		// lesender  Zugriff  auf Kernel-Daten.
		 	break;
		case 'k':
		 	x = *((unsigned int *)((unsigned int)1048576));			// lesender  Zugriff  auf Kernel-Code
		 	break; 
		case 'K':
		 	x = *exceptionstack; 									// lesender  Zugriff  auf Kernel-Stack.
		 	break;
		case 'g':
		 	x = *uart_addr; 										// lesender  Zugriff  auf Peripherie-Gerät,  z.B.  uart.
		 	break;
		case 'c':
		 	*((char*) user_prog) = x; 								// schreibender  Zugriff  auf  eigenen  Code.
		 	break;
		case 's':
		 	stackoverflow(x); 										
		 	break;
		case 'u':
		 	x = ((char*) 0xffffffff)[0]; 							//lesender  Zugriff  auf  nicht  zugeordnete  Adresse.
		 	break;
		case 'x':
		 	 ((void(*)(void)) arg)();  								// Sprung  auf  eigene  Daten  oder  Stack.
		 	break;
	}
		

		for(unsigned int i = 0; i < 10; i++) {
			syscall_putc(c);

			if( c >= 'A' && c <= 'Z') {
				for(volatile unsigned int j = 0; j < 1000000000; j++) {}
			} else {
				syscall_sleep(2);
			}
		}
}

void init() {
	for(;;) {
		char c = syscall_getc();
		syscall_thread(user_prog, &c, sizeof(c), 0);
	}
}

void thread_exit() {
	syscall_exit();
}
