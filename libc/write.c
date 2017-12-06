#include <unistd.h>
#include "sys/defs.h"
#include <libc.h>

ssize_t write(int fd, const void* buf, size_t len)
{
  return syscall3(fd, (uint64_t)buf, len, __NR_write);      
}

