.global _ivt

.global data_abort
.global Prefetch_abort
.global Undefined
.global supervisor
.global irq
.global fiq
.global reset

.global _init
.global do_abort
.global do_und

.balign 64
_ivt:
	b reset
	ldr pc, _undefined_instruction
	ldr pc, _supervisor
	ldr pc, _prefetch_abort
	ldr pc, _data_abort
	ldr pc, _not_used
	ldr pc, _irq
	ldr pc, _fiq


	_undefined_instruction: .word Undefined
	_supervisor:    		.word supervisor   
	_prefetch_abort:        .word prefetch_abort
	_data_abort:            .word data_abort
	_not_used:              .word not_used
	_irq:                   .word irq
	_fiq:                   .word fiq

_init:
	
	push {r0}						
	
	ldr r0, =_ivt
	mcr p15, 0, r0, c12, c0, 0 

	mrs r0, cpsr					//clearen des i bits
	BIC r0, r0, #0x80
	MSR CPSR_cxsf, r0

	ldr r0,= 0x08ff1000				//initialisierung der Stackpointer
	msr sp_abt, r0 

	ldr r0,= 0x08fe2000	
	msr sp_fiq, r0 

	ldr r0,= 0x08fd3000	
	msr sp_irq, r0 

	ldr r0,= 0x08fc4000	
	msr sp_und, r0

	ldr r0,= 0x08fb5000	
	msr sp_usr, r0			


	pop {r0}
	BX lr

do_abort:
	push {r0}
	mov r0,#0x1
	ldr r0,[r0]
	pop {r0}
    BX LR

do_und:
	.word 0xFFFFFFFF
    

reset:
	bx lr

fiq:
	bx lr

not_used:
	bx lr

data_abort :
	sub sp, sp, #0x38			/*38 für 14 register (14*4)56 in hex = 38*/
	stmdb sp!, {r0-r12, lr}     
	
	mov r0 , lr				
	sub r0, r0,#8
	BL firstline_da
	
	add sp, sp, #0x34
	
	ldr r1, [sp]
	sub r1, r1, #4
	str r1, [sp] 			

	sub sp, sp, #0x34
	
	mov r0 , sp
	mrs r1 , spsr 
	BL data_abort_c

    ldmfd sp!, {r0-r12,pc}^ 		
	
	bx lr

prefetch_abort :
	sub sp, sp, #0x38
	stmdb sp!, {r0-r12, lr}

	mov r0 , lr				
	sub r0, r0,#4
	BL firstline_pa

	mov r0 , sp
	mrs r1 , spsr
	BL Prefetch_abort_c

	
	ldmfd sp!, {r0-r12,pc}^ 
	bx lr

Undefined :
	sub sp, sp, #0x38
	stmdb sp!, {r0-r12, lr}

	mov r0 , lr
	sub r0, r0,#4
	BL firstline_und

	mov r0 , sp
	mrs r1 , spsr 
	BL undefined_c

	ldmfd sp!, {r0-r12,pc}^ 
	bx lr

supervisor :
	sub sp, sp, #0x38
	stmdb sp!, {r0-r12, lr}

	mov r0 , lr				
	sub r0, r0,#4
	BL firstline_spv
	

	mov r0 , sp
	mrs r1 , spsr
	BL supervisor_c


	ldmfd sp!, {r0-r12,pc}^  
	bx lr


.global set_registers
set_registers:
    msr sp_usr, r0
	msr lr_usr, r1
	msr SPSR_cxsf, r2

	bx lr



irq :	
	sub sp, sp, #0x38
	stmdb sp!, {r0-r12, lr}

	sub lr, lr, #4
	mov r0 , lr
	BL firstline_irq	

	add sp, sp, #0x34
	ldr r1, [sp]
	sub r1, r1, #4
	str r1, [sp] 
	sub sp, sp, #0x34

	mov r0 , sp
	BL irq_extra_handler 	

	mov r0 , sp
	mrs r1 , spsr
	BL irq_c

	ldmfd sp!, {r0-r12,pc}^ 
	bx lr

