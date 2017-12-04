.global writeSyscall

writeSyscall:
	int $0x80
	ret

