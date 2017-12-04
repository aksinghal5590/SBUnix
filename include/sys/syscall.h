#ifndef _SYS_CALL_H_
#define _SYS_CALL_H_
#include "sys/defs.h"

void userWrite(uint64_t fileDescriptor, char* data, uint64_t len);
void writeSyscall(uint64_t sysNum, uint64_t fd, uint64_t data, uint64_t len);
pid_t forkSyscall(uint64_t sysNum);
void systemCallHandler();
void sysHandler();
void systemWrite(uint64_t fd, uint64_t data, uint64_t len);
void systemRead(uint64_t fileDescriptor, uint64_t data, uint64_t len);
pid_t systemFork();
pid_t userFork();

#endif
