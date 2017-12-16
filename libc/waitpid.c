#include <stdio.h>
#include "sys/defs.h"
#include <libc.h>

pid_t waitpid(pid_t pid, int *status, int options)
{
	return syscall3(__NR_wait4, (uint64_t)pid, (uint64_t)status, (uint64_t)options);
}
