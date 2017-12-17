#ifndef _SYS_CALL_H_
#define _SYS_CALL_H_

#include "sys/defs.h"

#define __NR_read 0
#define __NR_write 1
#define __NR_open 2
#define __NR_close 3
#define __NR_yield 24
#define __NR_sleep 35
#define __NR_fork 57
#define __NR_execve 59
#define __NR_exit 60
#define __NR_wait4 61
#define __NR_getdents 78
#define __NR_getcwd 79
#define __NR_chdir 80
#define __NR_pipe 22
#define __NR_dup2 33
#define __NR_mmap 9
#define __NR_munmap 11
#define __NR_kill 62
#define __NR_ps 10

void initSyscalls();

void userWrite(uint64_t fileDescriptor, char* data, uint64_t len);
void writeSyscall(uint64_t sysNum, uint64_t fd, uint64_t data, uint64_t len);
pid_t forkSyscall(uint64_t sysNum);
void systemCallHandler();
void sysHandler();
void systemWrite(uint64_t fd, uint64_t data, uint64_t len);
void systemRead(uint64_t fileDescriptor, uint64_t data, uint64_t len);
pid_t systemFork();
void systemExit(uint64_t status);
void systemYield();
uint64_t systemExecvpe(char *file_path, char *argv[], char *envp[]);
uint64_t systemWaitPid(uint64_t pid, uint64_t status, uint64_t options);
void systemMunmap(uint64_t ptr);
uint64_t systemMMap(uint64_t size);
uint64_t systemSleep(uint64_t seconds);
void incrementSleepCount();
void systemKill(uint64_t killpid);
void systemProcList();
#endif
