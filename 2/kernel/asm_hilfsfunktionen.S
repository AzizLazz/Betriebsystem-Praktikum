.global _IFSR
.global _IFAR
.global _DFAR
.global _DFSR

.global sp_usr
.global lr_usr

.global sp_fiq
.global lr_fiq
.global spsr_fiq

.global sp_irq
.global lr_irq
.global spsr_irq

.global lr_svc
.global sp_svc
.global spsr_svc

.global lr_abt
.global sp_abt
.global spsr_abt

.global lr_und
.global sp_und
.global spsr_und

.global lr
.global cpsr
.global sp

lr:
	mov r0, lr
	bx lr

cpsr:
	mrs r0, cpsr
	bx lr

sp:
	mov r0, sp
	bx lr	


_IFSR :
	MRC p15, 0, r0 , c5, c0, 1
	BX lr

_IFAR :
	MRC p15, 0, r0 , c6, c0, 2
	BX lr

_DFSR :
	MRC p15, 0, r0, c5, c0, 0
	BX lr

_DFAR :
	MRC p15, 0, r0, c6, c0, 0
	BX lr


sp_usr:
	MRS r0, sp_usr
	BX lr
	

lr_usr:
	MRS r0, lr_usr
	BX lr

sp_fiq:
	MRS r0, sp_fiq
	BX lr
	
lr_fiq:
	MRS r0, lr_fiq
	BX lr
	
spsr_fiq:
	MRS r0, spsr_fiq
	BX lr


sp_irq:
	MRS r0, sp_irq
	BX lr
	
lr_irq:
	MRS r0, lr_irq
	BX lr
	
spsr_irq:
	MRS r0, spsr_irq
	BX lr


lr_svc:
	MRS r0, lr_svc
	BX lr

sp_svc:
	MRS r0, sp_svc
	BX lr

spsr_svc:
	MRS r0, spsr_svc
	BX lr

lr_abt:
	MRS r0, lr_abt
	BX lr

sp_abt:
	MRS r0, sp_abt
	BX lr

spsr_abt:
	MRS r0, spsr_abt
	BX lr

lr_und:
	MRS r0, lr_und
	BX lr

sp_und:
	MRS r0, sp_und
	BX lr

spsr_und:
	MRS r0, spsr_und
	BX lr