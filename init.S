.equ MODE_USR,      0x10
.equ MODE_FIQ,      0x11
.equ MODE_IRQ,      0x12
.equ MODE_SVC,      0x13
.equ MODE_ABT,      0x17
.equ MODE_UND,      0x1b
.equ MODE_SYS,      0x1f

.equ BIT_I,   0x80  @ when I bit is set, IRQ is disabled
.equ BIT_F,   0x40  @ when F bit is set, FIQ is disabled

.equ STACK_SIZE_UND,    0x00000000
.equ STACK_SIZE_SVC,    0x00000100
.equ STACK_SIZE_ABT,    0x00000000
.equ STACK_SIZE_FIQ,    0x00000100
.equ STACK_SIZE_IRQ,    0x00000100
.equ STACK_SIZE_USR,    0x00000100

#define STACK_SIZE_ISR (STACK_SIZE_UND + STACK_SIZE_SVC + STACK_SIZE_ABT + \
        STACK_SIZE_FIQ + STACK_SIZE_IRQ)

.section .data.isr
/* stack */
.global isr_stack_start
.global isr_stack_top

isr_stack_start:
.rept STACK_SIZE_ISR
.byte 0
.endr
isr_stack_top:

.text
.code 32
.global __vector_reset

.extern __bss_start__
.extern __bss_end__

/* reset entry */
__vector_reset:
    bl cpu_mmu_disable

    /* set the cpu to SVC32 mode and disable interrupt */
    mrs r0, cpsr
    bic r0, r0, #0x1f
    orr r0, r0, #0x13
    msr cpsr_c, r0

    /* setup stack */
    bl stack_setup

    /* clear .bss */
    mov     r0,#0                   /* get a zero                       */
    ldr     r1, =__bss_start         /* bss start                        */
    ldr     r2, =__bss_end           /* bss end                          */

bss_loop:
    cmp     r1, r2                   /* check if data to clear           */
    strlo   r0, [r1],#4              /* clear 4 bytes                    */
    blo     bss_loop                /* loop until done                  */

    /* call C++ constructors of global objects                          */
    ldr     r0, =__ctors_start__
    ldr     r1, =__ctors_end__

ctor_loop:
    cmp     r0, r1
    beq     ctor_end
    ldr     r2, [r0], #4
    stmfd   sp!, {r0-r1}
    mov     lr, pc
    bx      r2
    ldmfd   sp!, {r0-r1}
    b       ctor_loop
ctor_end:

    /* start artos Kernel */
    ldr     pc, _artos_main
_artos_main:
    .word artos_main

.globl cpu_mmu_disable
cpu_mmu_disable:
    mcr     p15, #0, r0, c8, c7, #0    @ invalidate tlb
    mrc     p15, #0, r0, c1, c0, #0
    bic     r0, r0, #1
    mcr     p15, #0, r0, c1, c0, #0    @ clear mmu bit
    dsb
    bx      lr

stack_setup:
    ldr     r0, =isr_stack_top

    @  Set the startup stack for svc
    mov     sp, r0

    @  Enter Undefined Instruction Mode and set its Stack Pointer
    msr     cpsr_c, #MODE_UND | BIT_I | BIT_F
    mov     sp, r0
    sub     r0, r0, #STACK_SIZE_UND

    @  Enter Abort Mode and set its Stack Pointer
    msr     cpsr_c, #MODE_ABT | BIT_I | BIT_F
    mov     sp, r0
    sub     r0, r0, #STACK_SIZE_ABT

    @  Enter FIQ Mode and set its Stack Pointer
    msr     cpsr_c, #MODE_FIQ | BIT_I | BIT_F
    mov     sp, r0
    sub     r0, r0, #STACK_SIZE_FIQ

    @  Enter IRQ Mode and set its Stack Pointer
    msr     cpsr_c, #MODE_IRQ | BIT_I | BIT_F
    mov     sp, r0
    sub     r0, r0, #STACK_SIZE_FIQ

    /* come back to SVC mode */
    msr     cpsr_c, #MODE_SVC | BIT_I | BIT_F
    bx      lr

.end
