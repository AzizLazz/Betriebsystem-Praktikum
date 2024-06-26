
#ifndef _IRQ_H_
#define _IRQ_H_

#include <arch/bsp/systimer.h>

/**
 * \file irq.h
 *
 * \brief Enthält Funktionen zum Konfigurieren des Irq Controllers des Raspi 2B
 */

/**
 * \brief Überprüft ob im ein uart irq am irq controller vorhanden ist
 * \return: 1 falls vorhanden.
 *          0 sonst.
 */
int irq_uart_pending(void);

/**
 * \brief Aktiviert irq's am irq controller für den uart.
 *
 * Damit uart interrupts auftreten muss der uart zusätlich
 * auf irq umgeschaltet werden
 */
void irq_uart_enable(void);

/**
 * \brief Deaktiviert irq's am irq controller für den uart.
 */
void irq_uart_disable(void);

/* ==================================================================
 * Achtung: Systimer Interrupts sind erst ab QEMU v 6.* verfügbar
 * ==================================================================
 */

/* Überprüft ob ein systimer irq am irq controller vorhanden ist.
 * Es werden alle vier systimer Register überprüft
 * @return: 1 falls mindestens einer vorhanden
 *          0 sonst
 */
unsigned int irq_systimer_pending(void);

/* Aktiviert den irq für den angegebenen systimer */
void irq_systimer_enable(enum SYS_TIMER_ID id);

/* Deaktiviert den irq für den angegebenen systimer */
void irq_systimer_disable(enum SYS_TIMER_ID id);
#endif
