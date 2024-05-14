#ifndef _CPU_H_
#define _CPU_H_

/**
 * \file cpu.h
 *
 * \brief Hilfreiche Funktionen für den arm Kern.
 */

#define _PSR_USR (0x10)
#define _PSR_IRQ (0x12)
#define _PSR_SVC (0x13)
#define _PSR_ABT (0x17)
#define _PSR_UND (0x1b)
#define _PSR_SYS (0x1f)

#ifndef __ASSEMBLY__
/**
 * \brief Aufgeschlüsselte Modusbits für PSR Register.
 */
enum PSR_MODE {
	PSR_USR = _PSR_USR,
	PSR_IRQ = _PSR_IRQ,
	PSR_SVC = _PSR_SVC,
	PSR_ABT = _PSR_ABT,
	PSR_UND = _PSR_UND,
	PSR_SYS = _PSR_SYS,
};

/**
 * \brief Extrahiert das Modusfeld des (C/S)PSR aus einem 32-Bit-Wert
 */
__attribute__((unused))
static enum PSR_MODE cpu_get_psr_mode(unsigned int psr){return psr&0x1f;}

/**
 * \brief Gibt den momentanen CPU Mode zurück
 */
enum PSR_MODE cpu_get_cpsr_mode();

/**
 * \ brief Lädt das gesamte CPSR Register und gibt es zurück
 */
unsigned int cpu_get_cpsr();

/**
 * \brief Setzt den Stack Pointer des angegebenen Modus auf den übergeben Wert
 */
void cpu_set_banked_sp(enum PSR_MODE mode, unsigned int val);

/**
 * \brief Gibt den Wert des Stack Pointer's des angegebenen Modus zurück
 */
unsigned int cpu_get_banked_sp(enum PSR_MODE mode);

/**
 * \brief Gibt den Wert des Link Register's des angegebenen Modus zurück
 */
unsigned int cpu_get_banked_lr(enum PSR_MODE mode);

/**
 * \brief Gibt den Wert des SPSR's des angegebenen Modus zurück
 */
unsigned int cpu_get_banked_spsr(enum PSR_MODE mode);

/**
 * \brief Aktiviert IRQs für den aktuellen Modus
 */
void cpu_enable_irq();

/**
 * \brief Deaktiviert IRQs für den aktuellen Modus
 */
void cpu_disable_irq();
#endif //__ASSEMBLY__
#endif
