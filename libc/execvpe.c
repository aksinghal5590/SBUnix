#include <libc.h>
#include <unistd.h> 
#include <stdio.h>
#include "sys/defs.h"

int execvpe(const char* filename, char *const argv[], char *const envp[]) {
        return syscall3(__NR_execve, (uint64_t)filename, (uint64_t)argv, (uint64_t)envp);
}

