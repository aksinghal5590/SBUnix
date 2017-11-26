#include "sys/defs.h"
#include "sys/syscall.h"
#include "sys/kprintf.h"

void* systemCallHandlerTable[2] = {NULL, systemWrite}; 

void userWrite(uint64_t fileDescriptor, char* data, uint64_t len)
{
	 writeSyscall(1, fileDescriptor, (uint64_t)data, len);
}

void writeSyscall(uint64_t sysNum, uint64_t fd, uint64_t data, uint64_t len)
{
	__asm__ volatile
	(
		"movq %[fd], %%rdi;"
		"movq %[data], %%rsi;"
		"movq %[len], %%rdx;"
		"movq %[sysNum], %%rax;"
		"int $0x80;"
		:
		:[fd]"g"(fd), [data]"g"(data), [len]"g"(len), [sysNum]"g"(sysNum)
		:"cc", "memory"
	);
}

void systemCallHandler()
{
	uint64_t sysNum;
	__asm__ volatile
	(
		"movq %%rax, %0;"
		:"=a"(sysNum)
		:
		:"cc","memory"
	);

	if(sysNum >= 0)
	{
		void* function_ptr = NULL;
		uint64_t ret;		
		__asm__ volatile
		(
			"pushq %%rdx;"
			:
			:
			:"cc", "memory"
		);
		function_ptr = systemCallHandlerTable[1] ;
		__asm__ volatile
		(
			"movq %%rax, %0;"
			"popq %%rdx;"
			"callq %%rax;"
			: "=a"(ret)
			: "r" (function_ptr)
			: "cc", "rcx", "memory"	
		);
		__asm__ volatile
		(
			"iretq;"
			:
			:
			: "cc", "memory"
		);
	}
}

void systemWrite(uint64_t fd, uint64_t data, uint64_t len)
{
	char* d = (char*)data;
	kprintf("Data is: %s\n", d);
}
