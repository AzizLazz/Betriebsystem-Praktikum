#ifndef _UART_H_
#define _UART_H_

/**
 * \file uart.h
 * \brief Implementierung des UART für den Raspi 2B
 *
 * Vor der Nutzung muss der UART mit der Funktion \p uart_init() initialisiert
 * werden. Die interne FIFO des UART ist deaktiviert (1 Byte tief).
 * Das Empfangen von Zeichen kann auf Interrupt Betrieb umgeschaltet werden.
 * Das Senden von Zeichen ist immer ohne Interrupt Betrieb.
 * Wenn der UART auf Interrupt Betrieb umgeschaltet wird, wird ein interner
 * Softwarebuffer benutzt um Zeichen zwischen zu speichern. Dazu muss bei
 * jedem UART Interrupt das angekommene Zeichen aus dem UART Data Register mit
 * Hilfe der Funktion \p uart_handle_irq() in den Softwarebuffer übertragen
 * werden.
 */


/**
 * \brief Initialisiert den UART
 *
 * Initialisiert den UART und aktiviert diesen. Interne FIFO wird deaktiviert
 * (1 Byte tief). Interrupts sind nach dem initialisieren deaktiviert.
 */
void uart_init(void);


/**
 * \brief Sendet ein Zeichen
 *
 * **Blockierend**
 *
 * Liest in einer aktiven Schleife das Flag Register
 * aus, ob ein Byte geschrieben werden kann und schreibt
 * dieses sobald möglich.
 *
 * \param chr Der zu sendende Charakter
 */
void uart_putc(char chr);


/**
 * \brief Liest ein Zeichen
 *
 * **Blockierend**
 *
 * Liest in einer aktiven Schleife das Flag Register aus bis ein Byte
 * gelesen werden kann und gibt dieses zurück.
 * Um blockierendes Lesen zu vermeiden kann zuerst mit \p uart_read_available
 * überprüft werden ob ein Byte vorhanden ist.
 * Falls der UART auf Interrupt Betrieb eingestellt ist wird das Zeichen aus
 * dem Softwarebuffer gelesen und nicht direkt aus dem Data Register des UART.
 *
 * \return Der empfangenen Charakter
 */
char uart_getc(void);


/**
 * \brief Liest ein Zeichen aus dem Buffer
 *
 * **Blockierend**
 *
 * Ist der UART nicht im Interrupt Betrieb verhält sich diese Funktion
 * identisch zu \p uart_getc.
 * Im Interrupt Betrieb wird das Zeichen aus dem internen Buffer ausgelesen
 * jedoch nicht aus dem Buffer entfernt. Damit wird bei dem nächsten
 * Lesevorgang das selbe Zeichen noch ein mal ausgelesen.
 *
 * \return Der empfangenen Charakter
 */
char uart_peekc(void);

/**
 * \brief Liest das letzte Zeichen aus dem Buffer
 *
 * **Blockierend**
 *
 * Ist der UART nicht im Interrupt Betrieb verhält sich diese Funktion
 * identisch zu \p uart_getc.
 * Im Interrupt Betrieb wird das letzte Zeichen aus dem internen Buffer ausgelesen
 * jedoch nicht aus dem Buffer entfernt. Damit wird bei dem nächsten
 * Lesevorgang das selbe Zeichen noch ein mal ausgelesen.
 *
 * \return Der empfangenen Charakter
 */
char uart_peekc_last(void);



/**
 * \brief Überprüft ob ein Byte zum lesen vorhanden ist
 *
 * Überprüft, ob ein Byte mit \p uart_getc gelesen werden
 * kann.
 * \return > 0 falls Byte vorhanden
 */
int uart_read_available();


 /**
 * \brief Aktiviert Interrupts am UART
 *
 * Hält den UART zwischenzeitig an, löscht alle
 * Interrupts und empfangenen bzw. zu versendenden Bytes
 * und **aktiviert** Interrupts am UART für zu empfangene Zeichen.
 * Noch nicht verarbeitet Bytes gehen verloren
 */
void uart_irq_enable();


/**
 * \brief Deaktiviert Interrupts am UART
 *
 * Hält den UART zwischenzeitig an, löscht alle
 * Interrupts und empfangenen bzw. zu versendenden Bytes
 * und **deaktiviert** Interrupts am UART für zu empfangene Zeichen.
 * Noch nicht verarbeitet Bytes gehen verloren.
 */
void uart_irq_disable();

/**
 * \brief Verarbeitet empfangenen Zeichen im Interrupt Betrieb
 *
 * Aufrufen um aufgetretenen UART Interrupt zu behandeln
 * Liest das angekommene Byte (falls vorhanden) und fügt
 * es dem Softwarebuffer hinzu. Falls der Buffer voll ist, wird
 * das Byte verworfen.
 *
 * \note Falls der UART nicht im Interrupt Betrieb ist, ist dies ein nop
 */
void uart_handle_irq();

#endif /* _UART_H_ */
