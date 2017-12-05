#ifndef _LIBC_H
#define _LIBC_H

#define BUF_SIZE 1024
#define O_RDONLY 00000000
#define O_DIRECTORY 00200000

#include "sys/defs.h"

int main();

void read(uint64_t fd, char* buf, uint64_t len);

void write(uint64_t fd, char* buf, uint64_t len);

int open(char* pathname, int flags);

int close(int fd);

int fork();

int execve(char* filename, char* argv[], char* envp[]);

void exit(int status);

void yield();

pid_t waitpid(pid_t pid, int *wstatus, int options);

//int getdents(int fd, char* dirp, int count);

//void getcwd(char* buf, int size);

//int chdir(char* path);

//int pipe(int* fd);

//int dup2(int a, int b);

void syscall0(uint64_t sysNum);

void syscall1(uint64_t sysNum, void* arg1);

void syscall1_i(uint64_t sysNum, int arg1);

void syscall1_ip(uint64_t sysNum, int* arg1);

void syscall2(uint64_t sysNum, void* arg1, int arg2);

void syscall2_ii(uint64_t sysNum, int arg1, int arg2);

void syscall_ici(uint64_t sysNum, int arg1, char* arg2, int arg3);

void syscall3(uint64_t arg1, uint64_t arg2, uint64_t arg3, uint64_t sysNum);

#endif
