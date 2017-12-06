.global writeSyscall
.global forkSyscall

writeSyscall:
	int $0x80
	ret

forkSyscall:
	movq %rdi, %rcx
	int $0x80
	ret
