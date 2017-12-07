.global irqDefault
.global irq0
.global irq1

.global divideByZero
.global tssFaul
.global gpfFault
.global pageFault
.global interrupt_common
.extern interrupt_handler

.extern irq0Handler
.extern irq1Handler

.macro PUSHA
    pushq %rdi
    pushq %rax
    pushq %rbx
    pushq %rcx
    pushq %rdx
    pushq %rbp
    pushq %rsi
    pushq %r8
    pushq %r9
    pushq %r10
    pushq %r11
    pushq %r12
    pushq %r13
    pushq %r14
    pushq %r15
.endm

.macro POPA
    popq %r15
    popq %r14
    popq %r13
    popq %r12
    popq %r11
    popq %r10
    popq %r9
    popq %r8
    popq %rsi
    popq %rbp
    popq %rdx
    popq %rcx
    popq %rbx
    popq %rax
    popq %rdi
.endm

divideByZero:
    cli
    pushq $0
    pushq $0
    jmp interrupt_common

tssFault:
    cli
    pushq $0
    pushq $10
    jmp interrupt_common

gpfFault:
    cli
    pushq $0
    pushq $13
    jmp interrupt_common

pageFault:
    cli
    pushq $14
    movq %cr2, %rax
    jmp interrupt_common

interrupt_common:
    PUSHA
    movq %rsp, %rdi
    callq interrupt_handler
    POPA
    add $0x10, %rsp
    sti
    iretq

irqDefault:
	iretq

irq0:
	cli
    PUSHA
    movq %rsp, %rdi
    callq irq0Handler
    POPA
    sti
    iretq

irq1:
        cli
        PUSHA    
        callq irq1Handler
        POPA
        sti
        iretq
