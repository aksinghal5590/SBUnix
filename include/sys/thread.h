#ifndef _THREAD_H_
#define _THREAD_H_

#include <stdarg.h>
#include <sys/defs.h>

struct PCB* createThread();
void threadInitialize_bin(char* binary_name);
void threadInitialize();
void performContextSwitch();
void firstUserProcess();
void threadATask();
void threadBTask();

#endif
