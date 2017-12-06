#include <stdio.h>
#include "sys/defs.h"
#include <libc.h>

void yield()
{
    syscall0(3);
}
