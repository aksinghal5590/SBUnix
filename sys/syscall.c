#include "sys/defs.h"
#include "sys/syscall.h"
#include "sys/kprintf.h"
#include "sys/pcb.h"

extern struct PCB* current_proc;

extern void writeSyscall(uint64_t fd, uint64_t data, uint64_t len, uint64_t sysNum);
extern void sysCallHandler();
extern void loadNextProcess();    
extern void getCharacters(uint64_t data, uint64_t len);

uint64_t* function_ptr = NULL;

void* systemCallHandlerTable[4] = {systemRead, systemWrite, systemExit, systemYield}; 

void userWrite(uint64_t fileDescriptor, char* data, uint64_t len)
{
	 writeSyscall(fileDescriptor, (uint64_t)data, len, 1);
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
