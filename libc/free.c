#include <stdio.h>
#include "sys/defs.h"
#include <libc.h>

void free(void* ptr)
{
	syscall1_i(__NR_munmap, (uint64_t)ptr);
}
