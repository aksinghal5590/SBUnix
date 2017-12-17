#define _GNU_SOURCE
#include <stdio.h>
#include <sys/defs.h>
#include <libc.h>

void proc_list() {
	syscall0(__NR_ps);
}