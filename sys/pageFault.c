#include "sys/kprintf.h"
#include "sys/pageFault.h"
#include "sys/interrupt.h"
#include "sys/defs.h"
#include "sys/pcb.h"

extern void walkUserPageTables(uint64_t userPml4Table, uint64_t vmaAddress);
void copyUserData(uint64_t pml4_add, uint64_t vmaAddress, uint64_t* vAddress, uint64_t len);

extern struct PCB *threadA;

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
        kprintf("!!!!Page Fault occured in Kernel!!!!!!");
    }
    else
    {
        struct vm_area_struct* vma = threadA->mm->vma_list;
        struct vm_area_struct* vmas = threadA->mm->vma_list;
        uint64_t startAdd, endAdd, startAdds, endAdds;
        while(vma != NULL)
        {
            startAdd = vma->start;
            endAdd = vma->end;
            startAdds = vmas->start;
            endAdds = vmas->end;
            if(cr2_val >= startAdd && cr2_val <= endAdd)
            {
                for(uint64_t i = startAdd; i < endAdd; i += 0x1000)
	            {
		            walkUserPageTables(cr3_val, i);
		            copyUserData(cr3_val, i,(uint64_t*) i, 4096);
	            }
                break;
            }
            if(cr2_val >= startAdds && cr2_val <= endAdds)
            {
                for(uint64_t i = startAdds; i < endAdds; i += 0x1000)
	            {
		            walkUserPageTables(cr3_val, cr2_val);
		            copyUserData(cr3_val, cr2_val,(uint64_t*) cr2_val, 4096);
	            }
                break;
            }
            vma = vma->next;
            vmas = vmas->next;
        }
        if(vma == NULL)
            faultPresent = 1;
    }
    if(faultPresent)
    {
        kprintf("Segmentation Fault Occured. Terminating the process \n");
    }
}
