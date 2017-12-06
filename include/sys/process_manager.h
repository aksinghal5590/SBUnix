#ifndef _PROCESS_MANAGER_H_
#define _PROCESS_MANAGER_H_

#include <stdarg.h>
#include <sys/pcb.h>
#include "sys/defs.h"

void switch_to_ring3_from_kernel();
void initIdleProcess();
void idleProcess();
void initializeProc(struct PCB* new_task, uint64_t entry, uint64_t stop);
void copyProcess(struct PCB* parent);
void loadNextProcess();

void addProcToReadyList(struct PCB* proc);
void addProcToSleepList(struct PCB* proc);
struct PCB* getNextReadyProc();
struct PCB* getNextSleepProc();
int checkReadyProcPresent();
void printReadyList();
void printSleepList();

#endif
