#ifndef _EXCEPTIONS_H_
#define _EXCEPTIONS_H_

/**
 * \file exceptions.h
 * \brief Funktionen zum behandeln verschiedener Exception auf ARM
 */

/**
 * \brief Zur Unterscheidung aus welchem Trampolin wir kommen
 *
 * Zur Unterscheidung aus welchen Trampolin wir kommen. Siehe \p exception()
 */
enum EXCEPTION_MODE{
	E_UND  = 1, ///< Undefined Instruction
	E_SVC  = 2, ///< Supervisor Call
	E_PABT = 3, ///< Prefetch Abort
	E_DABT = 4, ///< Data Abort
	E_IRQ  = 6  ///< Interrupt
};

/**
 * \brief Struct welches die Register auf dem Stack repräsentiert nach
 * dem Interrupt Trampolin
 */
struct regs{
	unsigned int r0;
	unsigned int r1;
	unsigned int r2;
	unsigned int r3;
	unsigned int r4;
	unsigned int r5;
	unsigned int r6;
	unsigned int r7;
	unsigned int r8;
	unsigned int r9;
	unsigned int r10;
	unsigned int r11;
	unsigned int r12;

	unsigned int sp; ///< SP des User/System Modus
	unsigned int lr; ///< LR des User/System Modus

	unsigned int psr; ///< PSR des vorherigen Modus (SPSR)

	unsigned int pc; ///< Rücksprungsadresse nach behandelter Exception
};

/**
 * \brief Setzt die IVT Adresse
 *
 * Aufrufen bevor die erste Exception auftretenen kann. Setzt die IVT Adresse
 * im VBAR Register des ARM cores
 */
void set_vector_base();

/**
 * \brief Diese Funktion wird aus dem IVT Trampolin angesprungen
 *
 * Diese Funktion muss selbst implementiert werden, damit diese beim linken
 * gefunden wird. Nachdem alle relevanten Register für den Kontext innerhalb
 * der Trampolin gesichert wurden, wird aus jedem Trampolin in diese Funktion
 * gesprungen.
 *
 * \param e gibt an aus welchem Trampolin exception aufgerufen wurde
 * \param r Kontext relevante Register
 */
extern void exception(enum EXCEPTION_MODE e, struct regs * r);

#endif /* _EXCEPTIONS_H_ */
