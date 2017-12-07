#include <libc.h>
#include <unistd.h>
#include "sys/defs.h"

int getdents(int fd, char* dirp, size_t count) {
        return syscall_ici(__NR_getdents, fd, dirp, count);
}
