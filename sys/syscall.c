#include "sys/defs.h"
#include "sys/syscall.h"
#include "sys/kprintf.h"
#include "sys/process_manager.h"
#include "sys/pcb.h"
#include "sys/vfs.h"

extern struct PCB* current_proc;

extern void writeSyscall(uint64_t fd, uint64_t data, uint64_t len, uint64_t sysNum);
extern void sysCallHandler();
extern void loadNextProcess();    

extern pid_t forkSyscall(uint64_t sysNum);

extern void getCharacters(uint64_t data, uint64_t len);

uint64_t* function_ptr = NULL;

extern struct PCB* current_proc;
void* systemCallHandlerTable[128];
// = {systemRead, systemWrite, systemExit, systemYield, systemFork};

void initSyscalls() {
	systemCallHandlerTable[__NR_read] = sys_read;
	systemCallHandlerTable[__NR_write] = sys_write;
	systemCallHandlerTable[__NR_open] = sys_open;
	systemCallHandlerTable[__NR_close] = sys_close;
	systemCallHandlerTable[__NR_yield] = systemYield;
	systemCallHandlerTable[__NR_fork] = systemFork;
	systemCallHandlerTable[__NR_exit] = systemExit;
	systemCallHandlerTable[__NR_getdents] = sys_getdents;
	systemCallHandlerTable[__NR_getcwd] = sys_getcwd;
	systemCallHandlerTable[__NR_chdir] = sys_chdir;
} 


void userWrite(uint64_t fileDescriptor, char* data, uint64_t len)
{
	 writeSyscall(fileDescriptor, (uint64_t)data, len, 1);
}


pid_t userFork()
{
	 return forkSyscall(4);
}

void systemCallHandler()
{
    uint64_t sysNum;
	__asm__ volatile
	(
		"pushq %rdx;"
	);
	__asm__ volatile
	(
		"movq %%rax, %0;"
		: "=r"(sysNum)
		:
		: "cc", "memory"
	);
	if(sysNum >= 0)
	{
		function_ptr = systemCallHandlerTable[sysNum];
		__asm__ volatile
		(
			"popq %%rdx;"
			"callq %0;"
			:
			: "r" (function_ptr)
			: "cc", "rcx", "memory"	
		);
	}
    __asm__ volatile
    (
        "iretq;"
        :
        :
        :"cc", "memory"
    );
}

void systemWrite(uint64_t fd, uint64_t data, uint64_t len)
{
	char* d = (char*)data;
	kprintf("Data is: %s\n", d);
	/*for(uint64_t i = 0; i < len; i++)
	{
		kprintf("%c", d++);
	}*/
}

void systemRead(uint64_t fileDescriptor, uint64_t data, uint64_t len)
{
    if(fileDescriptor==0)
    {
        if(len==0)
            return;
        getCharacters(data, len);
    }
}

pid_t systemFork()
{
    struct PCB *parent = current_proc; 
    struct PCB *child = copyProcess(parent); 
    initializeProc(child, parent->kstack[KSTACK_SIZE-6], parent->kstack[KSTACK_SIZE-3]);
    child->kstack[KSTACK_SIZE-7] = 0UL;
    kprintf("%d\n", child->pid);
    printReadyList();
    return child->pid;
}

void systemExit(uint64_t status)
{
    kprintf("Process exit with status: %d\n", status);
    current_proc->state = EXIT;
    loadNextProcess();
}

void systemYield()
{
    kprintf("Inside Yield\n");
    current_proc->state = READY;
    loadNextProcess();    
}
