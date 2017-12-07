#include "sys/defs.h"
#include "sys/elf64.h"
#include "sys/tarfs.h"
#include "sys/kprintf.h"
#include "sys/pcb.h"
#include "sys/userPageTable.h"
#include "sys/process_manager.h"
#include "sys/thread.h"
#include "sys/stdarg.h"

#define S_TOP 0xF0000000
#define S_SIZE 0x10000

struct PCB *userThread;

uint64_t read_file(char* file_name) {

    userThread = create_new_proc("User Process", 1); 
    uint64_t pml4_add = userThread->pml4;

    Elf64_Ehdr* eh = (Elf64_Ehdr*)read_tarfs(file_name);

    Elf64_Phdr* ph = (Elf64_Phdr*)((void*)eh + eh->e_phoff);
    uint64_t currentCR3;
    
	__asm__ volatile
	(
		"movq %%cr3, %0 \n\t"
		:"=r" (currentCR3)
		:
		:"cc", "memory"
	);

    kprintf("ph count %d\n", eh->e_phnum);
    updateUserCR3_Val((uint64_t)pml4_add);
    for (int i = 0; i < eh->e_phnum; ++i) {
        kprintf("%d\n",ph->p_type);
        if(ph->p_type == 1) {
           	kprintf("Vaddr: %x\n",ph->p_vaddr);
		insert_vma(userThread->mm, ph->p_vaddr, ph->p_vaddr + ph->p_memsz, ph->p_memsz, ph->p_flags, ph->p_type);
		mapUserPageTable((uint64_t)pml4_add, ph->p_vaddr, ph->p_vaddr+ph->p_memsz, (uint64_t*)eh+(ph->p_offset), ph->p_filesz);
	}
        ph += 1;
    }
    struct vm_area_struct *temp = userThread->mm->vma_list;
    while(temp->next != NULL)
    {
        temp = temp->next;
    }
    uint64_t endStackVAddress = S_TOP;
    uint64_t startStackVAddress = S_TOP - S_SIZE;
    insert_vma(userThread->mm, startStackVAddress, endStackVAddress, endStackVAddress-startStackVAddress, 1, 0);
   // mapUserPageTable((uint64_t)pml4_add, endStackVAddress-0x1000, endStackVAddress, (uint64_t*)(endStackVAddress-0x1000), 0x1000);
    updateUserCR3_Val(currentCR3);
    schedule_proc(userThread, eh->e_entry, endStackVAddress-0x8);
    return eh->e_entry;
}

void mapUserPageTable(uint64_t pml4_add, uint64_t startAddress, uint64_t endAddress, uint64_t* offset, uint64_t filesz)
{
	for(uint64_t i = startAddress; i < endAddress; i += 0x1000)
	{

		walkUserPageTables(pml4_add, i, 0);
	}
	copyUserData(pml4_add, startAddress, offset, filesz);
}
