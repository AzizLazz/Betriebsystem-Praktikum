#ifndef _KERNEL_H_
#define _KERNEL_H_

// Enthält allgemein Werte zum initialisieren
// Kann gerne geändert werden

#define KERNEL_MAX_ADDR (128 * (1024 * 1024)) // 128MB
#define KERNEL_MIN_ADDR (0)
#define EXCEPTION_MODE_STACK_SIZE (4 * ( 1024 )) // 4KB
#define THREADS_STACK_SIZE (1 * ( 1024 * 1024)) // 1MB

#define SVC_STACK_BASE ( KERNEL_MAX_ADDR - ( 0 * EXCEPTION_MODE_STACK_SIZE) )
#define ABT_STACK_BASE ( KERNEL_MAX_ADDR - ( 1 * EXCEPTION_MODE_STACK_SIZE) )
#define UDF_STACK_BASE ( KERNEL_MAX_ADDR - ( 2 * EXCEPTION_MODE_STACK_SIZE) )
#define IRQ_STACK_BASE ( KERNEL_MAX_ADDR - ( 3 * EXCEPTION_MODE_STACK_SIZE) )
#define SYS_STACK_BASE ( KERNEL_MAX_ADDR - ( 4 * EXCEPTION_MODE_STACK_SIZE) )

#define THREADS_STACK_BASE ( KERNEL_MAX_ADDR - ( 1024 * 1024) )

#endif // _KERNEL_H_
