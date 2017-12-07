#include "sys/defs.h"
#include <libc.h>
#include <unistd.h>

ssize_t read(int fd, void* buf, size_t count)
{
  return syscall_ici(__NR_read, fd, (char*)buf, count);
}
