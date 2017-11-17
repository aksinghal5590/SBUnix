#include "sys/kprintf.h"
#include "sys/stdarg.h"
#include "sys/defs.h"
#include "sys/pcb.h"
#include "sys/kernelLoad.h"
#include "sys/thread.h"

extern struct PCB *threadA, *threadB;
extern void initialSwitch(uint64_t first);
extern void schedule(uint64_t* first, uint64_t* second);

uint64_t threadA_fn = (uint64_t)&threadATask;
uint64_t threadB_fn = (uint64_t)&threadBTask;

struct PCB* createThread()
{
	return ((struct PCB*) kmalloc(sizeof(struct PCB)));
}

void threadInitialize()
{
	threadA->kstack[99] = threadA_fn;
	threadB->kstack[99] = threadB_fn;
	threadA->rsp = (uint64_t)&threadA->kstack[99];
	threadB->rsp = (uint64_t)&threadB->kstack[83];
}

void performContextSwitch()
{
	initialSwitch(threadA->rsp);
	schedule(&threadA->rsp, &threadB->rsp);
}

void threadATask()
{
	while(1)
	{
		kprintf("Currently in ThreadA\n");
		schedule(&threadA->rsp, &threadB->rsp);
	}
}

void threadBTask()
{
	while(1)
	{
		kprintf("Currently in ThreadB\n");
		schedule(&threadB->rsp, &threadA->rsp);
	}
}
