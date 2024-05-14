#ifndef _SCHEDULER_H_
#define _SCHEDULER_H_

#include <arch/cpu/exceptions.h>

/**
 * \file scheduler.h
 * \brief Funktionen zum verwalten des Schedulers und starten einzelner Threads.
 *
Insgesamt gibt es die Möglichkeit 32 Threads parallel aktiv zu verwalten
 * Der Idle Thread ist hierbei separat und hat immer die id -1. Der Scheduler
 * muss zuerst mit \p scheduler_init() initialisiert werden.
 */

/* Anzahl der verfügbaren Threads.*/
#define MAX_THREADS (32)
/* Idle Thread id.*/
#define IDLE_THREAD_ID (-1)

/**
 * \brief Alle States in denen sich ein Thred befinden kann
 */
enum thread_status {
	FINISHED, ///< Thread ist beendet
	READY,    ///< Thread ist bereit
	RUNNING,  ///< Thread is momentan aktiv
	IS_IDLE,  ///< Idle Thread
	EXTERN,   ///< Extern verwaltet
	SLEEPING, ///< Thread schläft
	RECV,     ///< Thread wartet auf Zeichen
};

/* Errors */
#define ERR_TCB_FULL      ((int) -1)
#define ERR_HOOK          ((int) -2)


/**
 * \brief Initialisiert den Scheduler
 *
 * Initialisiert alle internen TCB's des Scheduler. Der Scheduler
 * wird hierbei noch nicht gestartet. Um in den ersten Thread
 * einzusteigen, muss \ref scheduler_start benutzt werden.
 */
void scheduler_init();

/**
 * \brief Springt in den ersten Thread nachdem die Initialisierung
 *        beendet ist
 *
 * Diese Funktion kann nur aus dem Supervisor Modus aufgerufen werden
 * und springt nicht zurück, sondern initialisiert den ersten Thread
 * und startet diesen auch gleich.
 * Da hierbei nicht wie sonst durch das Eintreten und Verlassen einer
 * Exception der Kontextwechsel stattfindet muss der Stack Pointer für
 * den Supervisor Modus mit übergeben werden.
 * Die Funktion "repariert" diesen dann manuell, bevor in den User Modus
 * gewechselt wird
 *
 * \param func Die Funktion welche als erster Thread laufen soll.
 *             Falls NULL wird der idle Thread gestartet.
 * \param flags Flags für scheduler_on_new. Frei nutzbar.
 * \param svc_stack_base Stack Pointer Basis für den Supervisor Modus.
 */
void scheduler_start(void (*func) (),
                     unsigned int flags,
                     unsigned int svc_stack_base)
                     __attribute__((noreturn));

/**
 * \brief Kopiert die Argumente auf den Stack
 *
 * Kopiert die Argumente auf den Stack des Registerkontext.
 * regs->sp muss hierfür bereits gesetzt sein und wird anschließend
 * angepasst. regs->r0 wird auf die Argumente gesetzt.
 *
 * \param regs Registerkontext des Threads
 * \param args Zeiger auf die Argumente
 * \param arg_size Größe der Argumente in Bytes
 */
void scheduler_copy_args(struct regs * regs, const void * args, unsigned int arg_size);

/**
 * \brief Git die id des momentan aktiven Thread aus
 * \return Id des aktuellen aktiven Threads
 */
int scheduler_current();

/**
 * \brief Gibt die momentan im tcb abgespeicherten Register des
 *        Threads mit der angeforderten id zurück
 *
 * \param id Id des Threads
 * \return Struct mit allen gespeicherten Registerwerten
 *         NULL falls invalide id
 */
struct regs * scheduler_get_regs(int id);

/**
 * \brief Gibt den Status des Threads mit der angeforderten id zurück
 *
 * \param id Id des Threads.
 * \return Thread Status.
 *         ERROR bei invalider id.
 */
enum thread_status scheduler_get_state(int id);

/**
 * \brief Round Robin, alter Thread wird auf READY gesetzt
 *
 * Die Register auf dem Stack werden im TCB des jeweiligen Threads
 * abgespeichert und die Registerwerte des nächsten Threads aus dem TCB
 * auf den Stack geladen.
 * Dabei wird der alte Thread wieder zurück in die ready Warteschlange gelegt.
 *
 * \param regs Registerkontext des aktuellen Threads.
 * \return Id des nächsten Threads.
 */
int scheduler_round_robin(struct regs * regs);

/**
 * \brief Round Robin, alter Thread wird auf EXTERN gesetzt
 *
 * Wie scheduler_round_robin, aber der alte Thread wird nicht zurück in
 * die ready Warteschlange geladen und bekommt den Status: EXTERN.
 * Der Thread kann dann manuell verwaltet werden. Um den Thread wieder zurück
 * in die ready Warteschlange zu schieben können die Funktionen
 * scheduler_add_to_ready_[first,last] verwendet werden
 *
 * Falls der momentan aktive Thread bereits beendet wurde, verhält sich diese
 * Funktion wie scheduler_round_robin
 *
 * \param regs Registerkontext des aktuellen Threads.
 * \return Id des nächsten Threads.
 */
int scheduler_next(struct regs * regs);

/**
 * \brief Initialisiert einen neuen Thread und fügt diesen in die
 *        ready Warteschlange hinzu.
 *
 * Diese Funktion initialisiert einen neuen Threadkontext.
 * Hierbei wird alles initialisiert außer der Stackpointer und die
 * übergebenen Argumente.
 * Diese müssen in \ref scheduler_on_new() manuell gesetzt werden.
 *
 * Threads können nur erstellt werden, wenn der Scheduler aktiv ist,
 * also \ref scheduler_start() aufgerufen wurde.
 *
 * \param func Start Adresse des neuen Thrads
 * \param arg Pointer zu den Argumenten. Diese werden auf den Stack
 *            geladen und r0 des Threads passend gesetzt
 * \param arg_size Größe der Daten zu den arg zeigt.
 * \param flags wird an scheduler_on_new weiter gegeben. Frei nutzbar.
 * \return Neue id des angelegten Threads.
 *         ERR_TCB_FULL falls alle Threads belegt.
 *         ERR_HOOK falls error in \ref scheduler_on_new()
 *
 */
int scheduler_thread_create(void (* func) (void *),
                            const void *arg,
                            unsigned int arg_size,
                            unsigned int flags);

/**
 * \brief Setzt den Status des momentanen Threads auf FINISHED.
 *
 * Schlägt fehl, wenn der aktuelle Thread der Idle Thread ist.
 * Um den Kontextwechsel zum nächsten Thread durchzuführen, muss anschließend
 * scheduler_round_robin aufgerufen werden
 *
 * Panic's falls der aktuelle Thread der idle Thread ist.
 */
void scheduler_end_current(void);

/**
 * \brief Legt den aktuell laufenden Thread zum schlafen und lädt den nächsten Thread.
 *
 * Der aktuelle laufende Thread wird unterbrochen und in die sleep
 * Warteschlange eingefügt. Der Kontext des nächsten bereiten Threads wird
 * geladen. Ein interner Zähler wird auf auf den übergebenen Wert gesetzt.
 * Bei jedem Aufruf von \ref scheduler_update_sleep() wird dieser
 * reduziert.
 *
 * \param regs Momentaner Kontext auf dem Stack.
 * \param cycles Anzahl der Intervalle die geschlafen werden soll.
 * \return Id des nächsten Threads.
 */
int scheduler_sleep(struct regs * regs, unsigned int cycles);

/**
 * \brief Updated alle schlafenden Threads.
 *
 * Reduziert den internen Zähler aller schlafender Threads um 1. Threads
 * deren interner Zähler bei 0 ist, werden wieder an das Ende der ready
 * Warteschlange eingefügt.
 */
void scheduler_update_sleep(void);

/**
 * \brief Unterbricht den aktuell laufenden Thread bis ein Zeichen da ist.
 *
 * Der aktuell laufende Thread wird unterbrochen und wartet auf ein
 * Zeichen. Der Kontext des nächsten Threads wird geladen. Ein auf ein
 * Zeichen wartender Thread kann mit \ref scheduler_update_recv() wieder
 * aktiviert werden, vorausgesetzt ein Zeichen ist vorhanden.
 *
 * \param regs Momentaner Kontext auf dem Stack
 * \return Id des nächsten Threads
 */
int scheduler_recv(struct regs * regs);

/**
 * \brief Updated alle auf ein Zeichen wartende Threads.
 *in c
 * Liest so lange Zeichen aus dem internen Buffer des UART aus bis dieser
 * leer ist oder kein Thread mehr vorhanden ist der auf ein Zeichen wartet.
 * Threads die ein Zeichen erhalten haben werden wieder am Ende von der
 * ready Warteschlange eingefügt.
 */
void scheduler_update_recv(void);

/**
 * \brief Fügt einen Thread wieder in die interne Liste hinzu
 *
 * Folgende Funktionen fügen einen Thread wieder in die intern verwaltete
 * ready Warteschlange hinzu. Funktioniert nur wenn sich der Thread im
 * EXTERN state befindet. Bei Erfolgreichem Einfügen, wird der state des
 * Threads entsprechend angepasst. Der Thread wird am Ende der
 * Warteschlange eingefügt.
 *
 * Panic's falls invalide id oder Thread nicht im EXTERN state.
 *
 * \param id Id des Threads.
 */
void scheduler_add_to_ready_last(int id);

/**
 * \brief Fügt einen Thread wieder in die interne Liste hinzu
 *
 * Wie \ref scheduler_add_to_ready_last nur das der Thread am Anfang der
 * ready Warteschlange eingefügt wird
 */
void scheduler_add_to_ready_first(int id);


/**
 * \brief Hook für Verwaltung, etc...
 *
 * Diese Funktion wird bei einem Kontextwechsel aufgerufen nachdem
 * die Register auf dem Stack ausgetauscht wurden.
 * Die Funktion ist intern als __attribute__((weak)) definiert und macht nichts.
 * Folglich kann diese Funktion überschrieben werden.
 * Mögliche Anwendungen:
 * - Debugging
 * - Verwaltung
 *
 * \param old Id des vorherigen Threads
 * \param next Id des nächsten Threads
 */
extern void scheduler_on_schedule(int old, int next);

/**
 * \brief Hook für Verwaltung, etc..
 *
 *
 * Diese Funktion wird nach dem erstellen eines neuen Tasks durch
 * \ref scheduler_new_thread() aufgerufen. Kann verwendet werden um
 * nachträglich den Startkontext des neu erstellten Threads anzupassen.
 * Hier muss das Setzten des Stackpointers und das Kopieren der
 * Argumente behandelt werden.
 *
 * \param id Id des neuen Tasks
 * \param ctx Kontext des neu erstellten Tasks
 * \param arg Pointer auf die originalen Argumente
 * \param arg_size Größe der Argumente
 * \param flags Frei verwendbare flags
 * \return falls return ungleich 0, wird das erstellen des neuen Threads abgebrochen
 */
extern int scheduler_on_new(int id,
                            struct regs * ctx,
                            const void * arg,
                            unsigned int arg_size,
                            unsigned int flags);

/**
 * \brief Soll den Syscall zum beenden eines Threads aufrufen
 *
 * Diese Funktion muss selbst implementiert werden
 * Diese Funktion wird beim erstellen eines threads in das
 * lr Register geschrieben und soll den Thread vernünftig
 * beenden (per syscall!)
 * */
extern void thread_exit(void);
#endif
