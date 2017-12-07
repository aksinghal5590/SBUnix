#include <stdio.h>
#include "sys/defs.h"
#include <libc.h>

int execvpe(char *file_path, char *argv[], char *envp[])
{
return syscall3((uint64_t)file_path, (uint64_t)argv, (uint64_t)envp, 5);
}

