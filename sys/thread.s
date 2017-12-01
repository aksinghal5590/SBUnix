.global initialSwitch
.global schedule
.global switch_to_ring3
.extern userProcess_fn
.extern set_tss_rsp

initialSwitch:
	movq %rdi, %rsp
	ret

schedule:
	pushq %rax
	pushq %rbx
	pushq %rcx
	pushq %rdx
	pushq %rbp
	pushq %rsi
	pushq %rdi
	pushq %r8 
	pushq %r9 
	pushq %r10 
	pushq %r11
	pushq %r12
	pushq %r13 
	pushq %r14
	pushq %r15
	pushq %rdi
	movq %rsp, (%rdi)
	movq (%rsi), %rsp
	popq %rdi
	popq %r15
	popq %r14
	popq %r13	
	popq %r12
	popq %r11
	popq %r10
	popq %r9
	popq %r8
	popq %rdi
	popq %rsi
	popq %rbp
	popq %rdx
	popq %rcx
	popq %rbx
	popq %rax
	ret

switch_to_ring3:
	movq %rsp, %rax
	pushq $0x23
	pushq %rax
	pushfq
	pushq $0x2B
	pushq userProcess_fn
	movq %rsp, %rdi
	callq set_tss_rsp
	iretq
