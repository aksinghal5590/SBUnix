#include "sys/defs.h"
#include <libc.h>

void read(uint64_t fd, char* buf, uint64_t len)
{
  syscall3(0, fd, (uint64_t)buf, len);
}
