#include <stdio.h>
#include "sys/defs.h"
#include <libc.h>

void write(uint64_t fd, char* buf, uint64_t len)
{
  syscall3(1, fd, (uint64_t)buf, len);      
}

