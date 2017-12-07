#ifndef _LIBC_H
#define _LIBC_H

#define __NR_read 0
#define __NR_write 1
#define __NR_open 2
#define __NR_close 3
#define __NR_yield 24
#define __NR_fork 57
#define __NR_execve 59
#define __NR_exit 60
#define __NR_wait4 61
#define __NR_getdents 78
#define __NR_getcwd 79
#define __NR_chdir 80
#define __NR_pipe 22
#define __NR_dup2 33
#define BUF_SIZE 1024
#define O_RDONLY 00000000
#define O_DIRECTORY 00200000

#include "sys/defs.h"

int execve(char* filename, char* argv[], char* envp[]);

void exit(int status);

void yield();

//int dup2(int a, int b);

uint64_t syscall0(uint64_t sysNum);

int syscall1(uint64_t sysNum, void* arg1);

void syscall1_i(uint64_t sysNum, int arg1);

void syscall1_ip(uint64_t sysNum, int* arg1);

int syscall2(uint64_t sysNum, void* arg1, int arg2);

void syscall2_ii(uint64_t sysNum, int arg1, int arg2);

void syscall_ici(uint64_t sysNum, int arg1, char* arg2, int arg3);

int syscall3(uint64_t arg1, uint64_t arg2, uint64_t arg3, uint64_t sysNum);

#endif
