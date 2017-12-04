#include <stdio.h>
#include "sys/defs.h"
#include <libc.h>

void fork()
{
  syscall0(2);      
}

