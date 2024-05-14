#ifndef _SYSCALL_H_
#define _SYSCALL_H_

#define _SYSCALL_ID_EXIT   0
#define _SYSCALL_ID_GETC   1
#define _SYSCALL_ID_PUTC   2
#define _SYSCALL_ID_SLEEP  3
#define _SYSCALL_ID_THREAD 4
#define _SYSCALL_ID_KPRINT_CHAR 5
#define _SYSCALL_ID_KPRINT 6
#define _SYSCALL_ID_KPRINT_UINT 7
#define _SYSCALL_ID_GET_ID 8

#ifndef __ASSEMBLY__

enum SYSCAL_ID {
	EXIT = _SYSCALL_ID_EXIT,
	GETC = _SYSCALL_ID_GETC,
	PUTC = _SYSCALL_ID_PUTC,
	SLEEP = _SYSCALL_ID_SLEEP,
	THREAD = _SYSCALL_ID_THREAD,
	KPRINT_CHAR = _SYSCALL_ID_KPRINT_CHAR,
	KPRINT = _SYSCALL_ID_KPRINT,
	KPRINT_UINT = _SYSCALL_ID_KPRINT_UINT,
	GET_ID = _SYSCALL_ID_GET_ID
};

void __attribute__((noreturn)) syscall_exit();
char syscall_getc();
void syscall_putc(char c);
void syscall_sleep(unsigned int cycles);
void syscall_thread( void (*f) (void *), void * args, unsigned int arg_size, unsigned int flags);

void syscall_kprint_char(char *eingabe, ...);
void syscall_kprint(char *eingabe, ...);
void syscall_kprint_uint(char *eingabe, ...);
void syscall_get_id(char *eingabe, ...);

#endif // __ASSEMBLY__
#endif //_SYSCALL_H_
