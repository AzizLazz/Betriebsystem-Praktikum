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
#include <arch/cpu/mmu_asm.h>

#define USERSTACK_BEGIN_OFFSET (65536)
#define PROCESS_THREAD_START (12582912)
#define PROCESS_BASIS_ADDR (9437184)
#define PROCESS_SIZE (4194304)
#define PROCESS_STACK_BASE_1 (3149824)  //PROCESS_BASIS_ADDR + PROCESS_STACK_BASE_1 = letzte adresse vor guardpage
#define PROCESS_STACK_BASE_2 (3219456)  //PROCESS_BASIS_ADDR + i * PROCESS_SIZE für die anderen stacks
#define PROCESS_STACK_BASE_3 (3289088)
#define PROCESS_STACK_BASE_4 (3358720)  
#define PROCESS_NR (8)

enum SYS_TIMER_ID DEFAULT_TIMER = SYS_TIMER_C1;

__attribute__((weak)) void init() {
	panic("User init thread not properly linked");
}

//8x4 Array zur Prozessverwaltung, wird mit ids ersetzt
int addressraum[8][4] = {
						{-10,-10,-10,-10},
						{-10,-10,-10,-10},
						{-10,-10,-10,-10},
						{-10,-10,-10,-10},
						{-10,-10,-10,-10},
						{-10,-10,-10,-10},
						{-10,-10,-10,-10},
						{-10,-10,-10,-10}};

//hilfsvariablen:
int prcssnr = -1;
//int yeet = 0;
int newadressraum = 0;



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
		if (r->r7 == 0){					//syscall_exit, heißt globalcounter schleife ist zu Ende -> id wird entfernt 
			int id = scheduler_current();
			for (int i = 0; i < 8; i++) {
				for (int j = 0; j < 4; j++) {
					if (addressraum[i][j]==id)
					{
						addressraum[i][j]=-10;
					}
				}
    		}
		}
		else if (r->r7 == 5) 	//syscall print char
		{
			kprintf("%s%c",(char*)r->r0,r->r1);
		}
		else if (r->r7 == 6)	//syscall print string
		{
			kprintf("%s",(char*)r->r0);
		}
		else if (r->r7 == 7) 	//syscall print unsigned int
		{
			kprintf("%s%u",(char*)r->r0,r->r1);
		}
		else if (r->r7 == 8)  //syscall print id
		{
			kprintf("%s%u",(char*)r->r0,scheduler_current());
		}
		if (r->r7 != 5 && r->r7 != 6 && r->r7 != 7 && r->r7 != 8)	//normale behandlung der anderen syscalls
		{
			if (syscall_handle(r, TIMER_INTERVAL)) {
				break; 
			}
			kprintf("\nUnknown syscall id");	
		}
		
		
		// fall through
	case E_PABT:
	case E_DABT:
	case E_UND:
		if (r->r7 != 5 && r->r7 != 6 && r->r7 != 7 && r->r7 != 8)	//die syscall ids 5,6,7,8 sind nicht invalide
		{
			kprintf("\nInvalid operation. Killing thread with id: %i\n", curr_id);
			debug_full_dump(e, r);
			scheduler_end_current();
			scheduler_round_robin(r);
			break;
		}
		else{
			break;
		}
	
		
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



void scheduler_on_schedule(int prev, int next) {
	if (prev != next) {
		for (int i = 0; i < 8; i++) {
			for (int j = 0; j < 4; j++) {
				if (addressraum[i][j] == next)
				{
					tlbhandling();
					
					prcssnr = i;
					switchMapping(prcssnr);			//ändert das mapping der datasektion auf den aktuellen prozess
					break;
				}
			}
    	}
	}
}

int scheduler_on_new(int id,
                     struct regs * ctx,
                     const void * arg,
                     unsigned int arg_size,
                     unsigned int flags) {
	
	
	
	if (flags == 1){
		for (int i = 0; i < 8; i++) {
				if (addressraum[i][0] == newadressraum && addressraum[i][1] == -10)
				{
					prcssnr = i;
					addressraum[prcssnr][1] = id;
					break;
				}
				else if (addressraum[i][0] == newadressraum && addressraum[i][2] == -10 && addressraum[i][1] != -10)
				{
					prcssnr = i;
					addressraum[prcssnr][2] = id;
					break;
				}
    	}
		if(addressraum[prcssnr][1] == id){
			ctx->sp = (unsigned int)((PROCESS_STACK_BASE_2+USERSTACK_BEGIN_OFFSET) + ((prcssnr*PROCESS_SIZE) + PROCESS_BASIS_ADDR));
			initMapping(id,2,prcssnr);
		}

		else if (addressraum[prcssnr][2] == id){
			ctx->sp = (unsigned int)((PROCESS_STACK_BASE_3+USERSTACK_BEGIN_OFFSET) + ((prcssnr*PROCESS_SIZE) + PROCESS_BASIS_ADDR));
			initMapping(id,3,prcssnr);
		}
		scheduler_copy_args(ctx,arg,arg_size);
		return 0;
	}
	else if (flags == 2)	//behandelt den idle thread
	{
		ctx->sp = THREADS_STACK_BASE - (THREADS_STACK_SIZE * id);
		scheduler_copy_args(ctx,arg,arg_size);
		return 0;
	}
	else{
		//ein neuer freier addressraum wird gesucht
		newadressraum = id;
		for (int i = 0; i < 8; i++) {
				if (addressraum[i][0]==-10)
				{
					prcssnr = i;
					addressraum[prcssnr][0] = id;
					
					break;
				}
		}
		//neues laden der datasektion in den adressraum
		switchMapping(-1); 				
		load_user_data(prcssnr);
		
		//sp auf den ersten freien stack im adressraum
		ctx->sp = (unsigned int)((PROCESS_STACK_BASE_1+USERSTACK_BEGIN_OFFSET) + ((prcssnr*PROCESS_SIZE) + PROCESS_BASIS_ADDR));
	
		initMapping(id,1,prcssnr);
		
		scheduler_copy_args(ctx,arg,arg_size);
		return 0;
	}
	
	
}

void start_kernel(){
	uart_init();
	mmu_vorbi();				//initialisierung der MMU
	
	yellow_on();

	
	
	set_vector_base();
	irq_uart_enable();
	uart_irq_enable();

	systimer_set_interval(DEFAULT_TIMER, TIMER_INTERVAL);
	irq_systimer_enable(DEFAULT_TIMER);
	scheduler_init();
	
	
	kprintf("=== Betriebssystem gestartet ===\n");
	test_kernel();
	scheduler_start(init, 2, SVC_STACK_BASE);
	panic("reached end of kernel_start");
}
