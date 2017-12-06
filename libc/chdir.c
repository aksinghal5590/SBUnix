#include <libc.h>
#include <unistd.h>

int chdir(const char *path) {
        return syscall1(__NR_chdir, (void*)path);
}
