.equ MODE_USR,      0x10
.equ MODE_FIQ,      0x11
.equ MODE_IRQ,      0x12
.equ MODE_SVC,      0x13
.equ MODE_ABT,      0x17
.equ MODE_UND,      0x1b
.equ MODE_SYS,      0x1f


.equ BIT_I,   0x80  @ when I bit is set, IRQ is disabled
.equ BIT_F,   0x40  @ when F bit is set, FIQ is disabled



.macro CHANGE_TO_SVC
        msr     cpsr_c,#(BIT_F|BIT_I|MODE_SVC)
.endm

.macro CHANGE_TO_IRQ
        msr     cpsr_c,#(BIT_F|BIT_I|MODE_IRQ)
.endm


.global __vector_undefined
.global __vector_swi
.global __vector_prefetch_abort
.global __vector_data_abort
.global __vector_reserved
.global __vector_irq
.global __vector_fiq

.text
.code 32

__vector_undefined:
    nop
__vector_swi:
    nop
__vector_prefetch_abort:
    nop
__vector_data_abort:
    nop
__vector_reserved:
    nop
__vector_irq:
    nop
/*
    sub r14,r14,#4
    str r14,[r13,#-0x4]
    mrs r14,spsr
	str r14,[r13,#-0x8]
	str r0,[r13,#-0xc]
	mov r0,r13
	CHANGE_TO_SVC
	str r14,[r13,#-0x8]!
	ldr r14,[r0,#-0x4]
	str r14,[r13,#-0x4]
	ldr r14,[r0,#-0x8]
	ldr r0,[r0,#-0xc]
	stmdb r13!,{r0-r3,r14}
	bl common_irq_handler
	ldmia r13!,{r0-r3,r14}
	msr spsr,r14
	ldmfd r13!,{r14,pc}^
    */
__vector_fiq:
    nop
