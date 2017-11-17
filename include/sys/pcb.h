#ifndef _PCB_H_
#define _PCB_H_

#include <stdarg.h>
#include "sys/defs.h"

struct PCB {
   uint64_t kstack[100];
   uint64_t pid;
   uint64_t rsp;
   enum { RUNNING, SLEEPING, ZOMBIE } state;
   int exit_status;
};

#endif
