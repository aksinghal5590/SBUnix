#include <stdio.h>
#include "sys/defs.h"
#include <libc.h>

void* malloc(size_t size)
{
	uint64_t ret = syscall1_i(__NR_mmap, size);
	return ((void*)ret);
}
