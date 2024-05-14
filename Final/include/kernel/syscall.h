#ifndef _SYSCALL_H_
#define _SYSCALL_H_

#include <arch/cpu/exceptions.h>

#define SYSCALL_EXIT   (0)
#define SYSCALL_GETC   (1)
#define SYSCALL_PUTC   (2)
#define SYSCALL_SLEEP  (3)
#define SYSCALL_THREAD (4)
#define SYSCALL_KPRINT_CHAR (5)
#define SYSCALL_KPRINT (6)
#define SYSCALL_KPRINT_UINT (7)
#define SYSCALL_GET_ID (8)
/**
 * \file syscall.h
 * \brief Syscall Handler
 *
 * Syscall Handler zum bearbeiten aller bekannten Syscalls der binären Lösung.
 * Vorhandene Syscalls und Id:
 *
 * |Syscall | ID |
 * |:-------|---:|
 * | EXIT   | 0  |
 * | GETC   | 1  |
 * | PUTC   | 2  |
 * | SLEEP  | 3  |
 * | THREAD | 4  |
 * | KPRINT_CHAR | 5  |
 * | KPRINT| 6  |
 * | KPRINT_UINT | 7  |
 * | GET_ID | 8  |
 */

/**
 * \brief Behandelt alle aus der binären Lösung bekannte Syscalls.
 *
 * Nimmt einen Kontext entgegen und interpretiert diesen als Syscall.
 * Falls die Syscall id bekannt ist, wird der Syscall bearbeitet.
 * Dies beinhaltet auch das austauschen des Kontext falls notwendig.
 *
 * \param r Register Kontext des aktuellen Threads.
 * \timer_int Der Timer Intervall der neu gesetzt wird, falls ein
 *            Kontext Wechsel durch den Syscall entsteht
 * \return 1 falls erfolgreich, 0 bei Fehler oder unbekannter id.
 */
int syscall_handle(struct regs * r, unsigned int timer_int);

#endif
