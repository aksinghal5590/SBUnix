#include <stdio.h>
#include "sys/defs.h"
#include <libc.h>

int fork()
{
  return syscall0(4);
}

