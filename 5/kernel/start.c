/// Das vorherige Aufgabenblatt wurde als Demonstration wie die
/// binäre Lösung verwendet werden kann implementiert.
/// Die Implementierung darf/soll/muss abgeändert werden um das
/// nächste Aufgabenblatt zu implementieren.
/// Um die Demonstration nicht über zu viele Dateien zu verstreuen
/// ist alles in:
/// - kernel/start.c
/// - arch/cpu/entry.S
/// - user/main.c
/// zu finden.
/// Es bietet sich jedoch an manche Funktionalitäten in andere
/// Dateien aus zu lagern.

#include <arch/bsp/yellow_led.h>
#include <config.h>
#include <arch/bsp/uart.h>
#include <kernel/kprintf.h>
#include <kernel/panic.h>
#include <arch/cpu/exceptions.h>
#include <arch/bsp/systimer.h>
#include <arch/bsp/irq.h>
#include <kernel/debug.h>
#include <kernel/scheduler.h>
#include <kernel.h>
#include <stddef.h>
#include <kernel/syscall.h>
#include <arch/cpu/mmu_init.h>

enum SYS_TIMER_ID DEFAULT_TIMER = SYS_TIMER_C1;

__attribute__((weak)) void init() {
	panic("User init thread not properly linked");
}

void exception(enum EXCEPTION_MODE e, struct regs * r){
	enum PSR_MODE prev_mode = cpu_get_psr_mode(r->psr);
	if ( prev_mode != PSR_USR && prev_mode != PSR_SYS ) {
		debug_full_dump(e, r);
		panic("Exception taken from unexpected mode");
	}
	int curr_id = scheduler_current();
	switch (e) {
	case E_IRQ:
		if (irq_uart_pending()) {
			uart_handle_irq();
			volatile char c ; 
			switch (uart_peekc()) {
                case 'N':
					__asm__("mov r0, #0\n ldr r0, [r0]");   // lesender  Zugriff  auf  Null-Pointer.
                    break;

                case 'P':
					__asm__("mov pc, #0");					// Sprung  auf  Null-Pointer.
                    break;

                case 'C':
					*((char *) exception) = c;				// schreibender Zugriff auf eigenen Code.
                    break;

                case 'U':
					c = ((char*) 0xffffffff)[0];          // lesender Zugriff auf nicht zugeordnete Adresse
                    break;

                case 'X':
					init() ;								// Sprung auf User Code	
                    break;

                default:
                    break;
				}
			scheduler_update_recv();
			if (curr_id == IDLE_THREAD_ID) {
				scheduler_round_robin(r);
				systimer_set_interval(DEFAULT_TIMER, TIMER_INTERVAL);
			}
		}

		if (irq_systimer_pending()) {
			if (systimer_is_matched(DEFAULT_TIMER)) {
				scheduler_update_sleep();
				scheduler_round_robin(r);
				systimer_tick(DEFAULT_TIMER);
			}
		}
		break;
	case E_SVC:
		if (syscall_handle(r, TIMER_INTERVAL)) { break; }
		kprintf("\nUnknown syscall id");
		// fall through
	case E_PABT:
	case E_DABT:
	case E_UND:
		kprintf("\nInvalid operation. Killing thread with id: %i\n", curr_id);
		debug_full_dump(e, r);
		scheduler_end_current();
		scheduler_round_robin(r);
		break;
	default:
		panic("Invalid exception mode");
	}
}

__attribute__((weak)) void thread_exit(void) {
	panic("Could not properly link to exit syscall");
}

// Diese Funktionen werden vom Scheduler aufgerufen und erlauben es in das
// Erstellen von Threads bzw in den Kontextwechsel einzugreifen.
// Kann auch nur zum debuggen benutzt werden.
// Die Funktionen sind intern ohne Funktionalität implementiert und mit
// __attribute__((weak)) markiert und werden somit von eurer eigenen
// Implementierung "überschrieben"
// Für mehr Infos in die Dokumentation schauen ;)
//void scheduler_on_schedule(int prev, int next) {
//	if (prev != next) {
//		uart_putc('\n');
//	}
//}

int scheduler_on_new(int id,
                     struct regs * ctx,
                     const void * arg,
                     unsigned int arg_size,
                     unsigned int __attribute__((unused)) flags) {

	ctx->sp = THREADS_STACK_BASE - THREADS_STACK_SIZE * id;
	scheduler_copy_args(ctx, arg, arg_size);
	return 0;
}

void start_kernel(){
	mmu_vorbi();				//initialisierung der MMU
	yellow_on();

	uart_init();
	
	set_vector_base();
	irq_uart_enable();
	uart_irq_enable();

	systimer_set_interval(DEFAULT_TIMER, TIMER_INTERVAL);
	irq_systimer_enable(DEFAULT_TIMER);
	scheduler_init();
	
	
	kprintf("=== Betriebssystem gestartet ===\n");
	test_kernel();
	scheduler_start(init, 0, SVC_STACK_BASE);
	panic("reached end of kernel_start");
}
