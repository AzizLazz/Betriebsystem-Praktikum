#ifndef _SYSTIMER_H_
#define _SYSTIMER_H_

/* Stellt die vier unterschiedliche compare Register des
 * Systimers dar
 * Hinweis:
 * Unter Qemu kann jede Timer Id benutzt werden. Auf der
 * Hardware sind jedoch SYS_TIMER_C0 und SYS_TIMER_C2 für
 * die GPU reserviert. Wer sowohl mit der Hardware als auch
 * QEMU kompatibel bleiben möchte sollte sich auf SYS_TIMER_C1
 * und SYS_TIMER_C3 beschränken
 */
enum SYS_TIMER_ID {
	SYS_TIMER_C0 = 0,
	SYS_TIMER_C1 = 1,
	SYS_TIMER_C2 = 2,
	SYS_TIMER_C3 = 3,
};

typedef struct _timer_frc {
	// Untere 32 bits des 64 bit free running counter
	unsigned int lo;
	// Obere 32 bits des 64 bit free running counter
	unsigned int hi;
} timer_frc;

/* Liest den aktuellen Wert des Free Running Counter und schreibt
 * diesen in den übergebenen Pointer
 */
void systimer_get_frc(timer_frc *);

/* \brief Setzt das Interval für den angegebenen Timer
 * Cleart den Match (falls vorhanden) und setzt das
 * Compare Register auf frc.lo + interval.
 * Das Interval wird intern vermerkt und für systimer_tick
 * benutzt
 */
void systimer_set_interval(enum SYS_TIMER_ID, unsigned int interval);

/* \brief Cleart den Match (falls vorhanden) und updated das Interval
 * Hierbei wird der letzte vorhandene Wert im compare Register
 * benutzt und auf diesen das gespeicherte Interval
 * hinzuaddiert
 */
void systimer_tick(enum SYS_TIMER_ID);

/* Gibt and ob ein Match für das angegebene Compare Register
 * aufgetreten ist
 * @return:
 *	1 falls aufgetreten
 *	0 sonst
 */
unsigned int systimer_is_matched(enum SYS_TIMER_ID);

/* Cleart den Match (falls vorhanden) für das angegebene Compare
 * Register
 */
void systimer_clear_match(enum SYS_TIMER_ID);

#endif
