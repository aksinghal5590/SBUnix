#include "sys/kprintf.h"
#include "sys/pageFault.h"
#include "sys/interrupt.h"
#include "sys/defs.h"
#include "sys/pcb.h"
#include "sys/utility.h"
#include "sys/freelist.h"
#include "sys/userPageTable.h"

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

void pageFaultHandler(registers_t regSet)
{
    int faultPresent = 0;
    uint64_t error_code = regSet.err_number;
	
	uint64_t cr2_val, cr3_val;
	__asm__ volatile
	(
		"movq %%cr2, %0 \n\t"
		:"=r" (cr2_val)
		:
		:"cc", "memory"
	);
	// kprintf("Value of CR2 is: %x PID: %d\n", cr2_val, current_proc->pid);
	__asm__ volatile
	(
		"movq %%cr3, %0 \n\t"
		:"=r" (cr3_val)
		:
		:"cc", "memory"
	);

    if(cr2_val >= VIRTUAL_BASE)
    {
        ERROR("!!!!Page Fault occured in Kernel!!!!!!. Terminating the Process.");
    }
    else if(error_code & 0x1)
    {
       uint64_t* pt_val = getPTTableEntry(cr3_val, cr2_val);
       if(!((*pt_val) & 0x2) && ((*pt_val) & 0x4000000000000000))
       {
            struct PAGE* p = getPageStruct(*pt_val);
            if(p->use_cnt > 1)
            {
                uint64_t pg = getPage();
                uint64_t *vAddress = (uint64_t*)(VIRTUAL_BASE + pg);
                for(int i = 0; i < 512; i++)
                {
                    *(vAddress + i) = 0x0;
                }
		uint64_t cr2_val_aligned = ((cr2_val>>12)<<12);
                memcpy((void*) vAddress, (void*)cr2_val_aligned, 0x1000);
                useExistingPage(cr3_val, cr2_val_aligned, pg);
                p->use_cnt--;
            }
            else
            {
                *pt_val = *pt_val | 0x2;
                *pt_val = *pt_val | 0xBFFFFFFFFFFFFFFF;
            }
       }
       else
       {
            ERROR("Segmentation Fault Occured. Terminating the process \n");
       }
    }
    else
    {
        struct vm_area_struct* vma = current_proc->mm->vma_list;
        uint64_t startAdd, endAdd;

        while(vma != NULL)
        {
	    if(vma->type != 20)
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
