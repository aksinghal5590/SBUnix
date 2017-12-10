#include "sys/kprintf.h"
#include "sys/stdarg.h"
#include "sys/defs.h"
#include "sys/elf64.h"
#include "sys/pcb.h"
#include "sys/kernelLoad.h"
#include "sys/thread.h"
#include "sys/syscall.h"
#include <sys/interrupt.h>

extern struct PCB *threadA, *threadB;
extern void initialSwitch(uint64_t first);
extern void schedule(uint64_t* first, uint64_t* second);
extern void switch_to_ring3();

uint64_t threadA_fn = (uint64_t)&threadATask;
uint64_t threadB_fn = (uint64_t)&threadBTask;
uint64_t userProcess_fn;// = (uint64_t)&firstUserProcess;

struct PCB* createThread()
{
	return ((struct PCB*) kmalloc(sizeof(struct PCB)));
}

// void threadInitialize_bin(char* binary_name)
// {
//         threadA->kstack[99] = read_file(binary_name, NULL, NULL);
//         threadB->kstack[99] = threadB_fn;
//         threadA->rsp = (uint64_t)&threadA->kstack[99];
//         threadB->rsp = (uint64_t)&threadB->kstack[83];
// }

void threadInitialize()
{
	threadA->kstack[99] = threadA_fn;
	threadB->kstack[99] = threadB_fn;
	threadA->rsp = (uint64_t)&threadA->kstack[99];
	threadB->rsp = (uint64_t)&threadB->kstack[83];
}

void performContextSwitch(uint64_t eEntry)
{
	userProcess_fn = eEntry;
	initialSwitch(threadA->rsp);
}

void firstUserProcess()
{
	kprintf("This is the first user process at: %x\n", &firstUserProcess);
	userWrite(1, "Vaibhav Rustagi", 5);
	while(1)
	{
		;
	}
}

void threadATask()
{
	int i = 0;
	while(i < 1)
	{
		kprintf("Currently in ThreadA\n");
		schedule(&threadA->rsp, &threadB->rsp);
		i++;
	}
	kprintf("Check by entering ring 3\n");
	switch_to_ring3();
	kprintf("Returned from ring 3\n");
	while(1)
	{
		;
	}
} 

void threadBTask()
{
	kprintf("Currently in ThreadB\n");
	schedule(&threadB->rsp, &threadA->rsp);
}
