#include "sys/utility.h"
#include "sys/kprintf.h"

void loadCR3(uint64_t pml4)
{
	__asm__(
		"movq %0, %%cr3 \n\t"
		:
		:"r"(pml4)
		:"cc"
	);
    __asm__("sti");
}

uint64_t getCR2Val()
{
    uint64_t cr2_val;
	__asm__ volatile
	(
		"movq %%cr2, %0 \n\t"
		:"=r" (cr2_val)
		:
		:"cc", "memory"
	);
	kprintf("Value of CR2 is: %x\n", cr2_val);
    return cr2_val;
}

uint64_t getCR3Val()
{
    uint64_t cr3_val;
	__asm__ volatile
	(
		"movq %%cr3, %0 \n\t"
		:"=r" (cr3_val)
		:
		:"cc", "memory"
	);
	kprintf("Value of CR3 is: %x\n", cr3_val);
    return cr3_val;
}

void flushTLB()
{
    uint64_t cr3_val;
	__asm__ volatile
	(
		"movq %%cr3, %%rax;"
		"movq %%rax, %%cr3;"
		"movq %%cr3, %0;"
		:"=r"(cr3_val)
		:
		:"cc", "memory"
	);
	kprintf("Value of CR3 is: %x\n", cr3_val);
}

void ERROR(char* error)
{
    kprintf("%s. Please Reboot the System.\n", error);
    //TODO exit the process
    //__asm__ ("sti");
    while(1);
}
