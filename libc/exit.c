#include <stdio.h>
#include "sys/defs.h"
#include <libc.h>

void exit(int status)
{
    syscall1_i(__NR_exit, status);
}
