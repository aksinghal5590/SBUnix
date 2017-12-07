#include "sys/kprintf.h"
#include "sys/pageFault.h"
#include "sys/interrupt.h"
#include "sys/defs.h"
#include "sys/pcb.h"
#include "sys/utility.h"

extern void walkUserPageTables(uint64_t userPml4Table, uint64_t vmaAddress, uint64_t oldPhyAddress);

extern struct PCB *userThread;
extern struct PCB* current_proc;

void divideByZeroHandler()
{
    ERROR("Divide by zero error occurs. Terminating the process\n");
}

void tssFaultHandler()
{
    ERROR("TSS Error occured. Terminating the process.\n");
}

void gpfFaultHandler()
{
    ERROR("General Fault Protection occured. Terminating the process.\n");
}

void pageFaultHandler()
{
    int faultPresent = 0;
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

    if(cr2_val >= VIRTUAL_BASE)
    {
        ERROR("!!!!Page Fault occured in Kernel!!!!!!. Terminating the Process.");
    }
    else
    {
        struct vm_area_struct* vma = userThread->mm->vma_list;
        uint64_t startAdd, endAdd;
        while(vma != NULL)
        {
            startAdd = vma->start;
            endAdd = vma->end;
            if(cr2_val >= startAdd && cr2_val <= endAdd)
            {
                for(uint64_t i = startAdd; i <= endAdd; i += 0x1000)
	            {
		            walkUserPageTables(cr3_val, i, 0);
	            }
                break;
            }
            vma = vma->next;
        }
        if(vma == NULL)
            faultPresent = 1;
    }
    if(faultPresent)
    {
        ERROR("Segmentation Fault Occured. Terminating the process \n");
    }
}
