#define __ASSEMBLY__
#include <user/syscalls.h>
#undef __ASSEMBLY__

.macro SYSCALL name id
.global \name
\name:
	push {r7,lr}
	mov r7, #(\id)
	svc #0
	pop {r7, pc}
.endm

SYSCALL syscall_exit _SYSCALL_ID_EXIT
SYSCALL syscall_getc _SYSCALL_ID_GETC
SYSCALL syscall_putc _SYSCALL_ID_PUTC
SYSCALL syscall_sleep _SYSCALL_ID_SLEEP
SYSCALL syscall_thread _SYSCALL_ID_THREAD
SYSCALL syscall_kprint_char _SYSCALL_ID_KPRINT_CHAR
SYSCALL syscall_kprint _SYSCALL_ID_KPRINT
SYSCALL syscall_kprint_uint _SYSCALL_ID_KPRINT_UINT
SYSCALL syscall_get_id _SYSCALL_ID_GET_ID