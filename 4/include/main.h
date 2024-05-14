#ifndef USERTHREAD_H
#define USERTHREAD_H
#include <kernel/kprintf.h>

void busy_wait_loop(void);
void syscall_undef(void);
void syscall_transmit(char c);
char syscall_recieve(void);
void syscall_sleep(unsigned int time);
void syscall_createthread(void(* func)(void *),const void *args,unsigned int arg_size);
void syscall_endthread(void);

void worker_thread(void *arg);

void main(void *args);

#endif//USERTHREAD.h