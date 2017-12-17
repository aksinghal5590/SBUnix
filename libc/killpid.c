#include <stdio.h>
#include "sys/defs.h"
#include <libc.h>

void kill(uint64_t killpid)
{
	syscall1_i(__NR_kill, killpid);
}
