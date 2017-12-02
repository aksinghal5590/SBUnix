#include "sys/kprintf.h"
#include "sys/pageFault.h"

extern void walkUserPageTables(uint64_t userPml4Table, uint64_t vmaAddress);

void pageFaultHandler()
{
	kprintf("Currently in Page Fault Handler\n");
	uint64_t cr2_val, cr3_val;
	__asm__ volatile
	(
		"movq %%cr2, %0 \n\t"
		:"=r" (cr2_val)
		:
		:"cc", "memory"
	);
	kprintf("Value of CR2 is: %x\n", cr2_val);
	__asm__ volatile
	(
		"movq %%cr3, %0 \n\t"
		:"=r" (cr3_val)
		:
		:"cc", "memory"
	);
	kprintf("Value of CR3 is: %x\n", cr3_val);
	walkUserPageTables(cr3_val, cr2_val);	
}
