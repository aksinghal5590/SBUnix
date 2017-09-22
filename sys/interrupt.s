.global irq0
.global irq1

.extern irq0Handler
.extern irq1Handler

irq0:
	cli
	pushq %rax
	pushq %rcx
	pushq %rdx
	pushq %r8
	pushq %r9
	pushq %r10
	callq irq0Handler
	popq %r10
	popq %r9
	popq %r8
	popq %rdx
	popq %rcx
	popq %rax
 	iretq

irq1:
        cli
        pushq %rax
        pushq %rcx
        pushq %rdx
        pushq %r8
        pushq %r9
        pushq %r10
        callq irq1Handler
        popq %r10
        popq %r9
        popq %r8
        popq %rdx
        popq %rcx
        popq %rax
        iretq
