.global userWriteTarfs

userWriteTarfs:
	int $0x80
	ret
