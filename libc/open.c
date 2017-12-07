#include <libc.h>
#include <unistd.h>

int open(const char *pathname, int flags) {
    return (int)syscall2(2, (void*)pathname, flags);
}
