#include <stdio.h>
#include "sys/defs.h"
#include <libc.h>

int fork()
{
  syscall0(2);      
}

