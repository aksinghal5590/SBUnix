#include "sys/defs.h"
#include <libc.h>
#include <unistd.h>

ssize_t read(int fd, void* buf, size_t count)
{
  return syscall3(fd, (uint64_t)buf, count, __NR_read);
}
