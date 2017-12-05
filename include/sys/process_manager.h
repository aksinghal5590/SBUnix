#ifndef _PROCESS_MANAGER_H_
#define _PROCESS_MANAGER_H_

#include <stdarg.h>
#include <sys/pcb.h>
#include "sys/defs.h"


void init_idle_process();
void schedule_proc(struct PCB* new_task, uint64_t entry, uint64_t stop);
void copyProcess(struct PCB* parent);
void schedule_next_process();
#endif
