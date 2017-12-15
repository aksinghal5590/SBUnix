#include <libc.h>
#include <unistd.h> 
#include "sys/defs.h"

int execvpe(const char* filename, char *const argv[], char *const envp[]) {
        return syscall3((uint64_t)filename, (uint64_t)argv, (uint64_t)envp, __NR_execve);
}

