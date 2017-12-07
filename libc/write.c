#include <unistd.h>
#include "sys/defs.h"
#include <libc.h>

ssize_t write(int fd, const void* buf, size_t len)
{
  return syscall_ici(__NR_write, fd, (char*)buf, len);      
}

