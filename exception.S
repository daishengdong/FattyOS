#include <asm.h>


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


.syntax unified
.text

/* exception handling glue.
 * NOTE: only usable on armv6+ cores
 */

/* macros to align and unalign the stack on 8 byte boundary for ABI compliance */
.macro stack_align, tempreg
    /* make sure the stack is aligned */
    mov     \tempreg, sp
    tst     sp, #4
    subeq   sp, #4
    push    { \tempreg }

    /* tempreg holds the original stack */
.endm

.macro stack_restore, tempreg
    /* restore the potentially unaligned stack */
    pop     { \tempreg }
    mov     sp, \tempreg
.endm

/* save and disable the vfp unit */
.macro vfp_save, temp1
    /* save old fpexc */
    vmrs    \temp1, fpexc

    push    { \temp1 }

    /* hard disable the vfp unit */
    bic     \temp1, #(1<<30)
    vmsr    fpexc, \temp1
.endm

/* restore the vfp enable/disable state */
.macro vfp_restore, temp1
    /* restore fpexc */
    pop     { \temp1 }

    vmsr    fpexc, \temp1
.endm

/* Save callee trashed registers.
 * At exit r0 contains a pointer to the register frame.
 */
.macro save
    /* save spsr and r14 onto the svc stack */
    srsdb   #0x13!

    /* switch to svc mode, interrupts disabled */
    cpsid   i,#0x13

    /* save callee trashed regs and lr */
    push    { r0-r3, r12, lr }

    /* save user space sp/lr */
    sub     sp, #8
    stmia   sp, { r13, r14 }^

#if ARM_WITH_VFP
    /* save and disable the vfp unit */
    vfp_save    r0
#endif

    /* make sure the stack is 8 byte aligned */
    stack_align r0

    /* r0 now holds the pointer to the original iframe (before alignment) */
.endm

.macro save_offset, offset
    sub     lr, \offset
    save
.endm

.macro restore
    /* undo the stack alignment we did before */
    stack_restore r0

#if ARM_WITH_VFP
    /* restore the old state of the vfp unit */
    vfp_restore r0
#endif

    /* restore user space sp/lr */
    ldmia   sp, { r13, r14 }^
    add     sp, #8

    pop     { r0-r3, r12, lr }

    /* return to whence we came from */
    rfeia   sp!
.endm

/* Save all registers.
 * At exit r0 contains a pointer to the register frame.
 */
.macro saveall
    /* save spsr and r14 onto the svc stack */
    srsdb   #0x13!

    /* switch to svc mode, interrupts disabled */
    cpsid   i,#0x13

    /* save all regs */
    push    { r0-r12, lr }

    /* save user space sp/lr */
    sub     sp, #8
    stmia   sp, { r13, r14 }^

#if ARM_WITH_VFP
    /* save and disable the vfp unit */
    vfp_save    r0
#endif

    /* make sure the stack is 8 byte aligned */
    stack_align r0

    /* r0 now holds the pointer to the original iframe (before alignment) */
.endm

.macro saveall_offset, offset
    sub     lr, \offset
    saveall
.endm

.macro restoreall
    /* undo the stack alignment we did before */
    stack_restore r0

#if ARM_WITH_VFP
    /* restore the old state of the vfp unit */
    vfp_restore r0
#endif

    /* restore user space sp/lr */
    ldmia   sp, { r13, r14 }^
    add     sp, #8

    pop     { r0-r12, r14 }

    /* return to whence we came from */
    rfeia   sp!
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


FUNCTION(__vector_irq)
	save_offset    #4

	/* r0 now holds pointer to iframe */

	/* track that we're inside an irq handler */
	ldr r2, =__arm_in_handler
	mov 	r1, #1
	str 	r1, [r2]

	/* call into higher level code */
	bl	platform_irq

	/* clear the irq handler status */
	ldr r2, =__arm_in_handler
	mov 	r2, #0
	str 	r2, [r1]

	/* reschedule if the handler returns nonzero */
	cmp 	r0, #0
	movne	r0, #1

	/* currently no threading */
	/* blne	thread_preempt */

	restore

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

.data
DATA(__arm_in_handler)
	.word	0	
