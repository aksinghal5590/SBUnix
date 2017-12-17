#include <unistd.h>
#include <libc.h>
#include <stdlib.h>
#include <stdio.h>

unsigned int sleep(unsigned int seconds) {
	return syscall1_i(__NR_sleep, seconds);
}
