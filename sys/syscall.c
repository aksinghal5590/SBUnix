#include "sys/defs.h"
#include "sys/syscall.h"
#include "sys/kprintf.h"

extern void writeSyscall(uint64_t fd, uint64_t data, uint64_t len, uint64_t sysNum);

uint64_t* function_ptr = NULL;

void* systemCallHandlerTable[2] = {NULL, systemWrite}; 

void userWrite(uint64_t fileDescriptor, char* data, uint64_t len)
{
	 writeSyscall(fileDescriptor, (uint64_t)data, len, 1);
}

void systemCallHandler()
{
	uint64_t sysNum;
	__asm__ volatile
	(
		"movq %%rcx, %0;"
		: "=r"(sysNum)
		:
		: "cc", "memory"
	);
	if(sysNum >= 0)
	{
		__asm__ volatile
		(
			"pushq %rdx;"
		);
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
		: "cc", "memory"
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
