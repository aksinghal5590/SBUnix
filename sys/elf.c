#include "sys/defs.h"
#include "sys/elf64.h"
#include "sys/tarfs.h"
#include "sys/kprintf.h"
#include "sys/pcb.h"
#include "sys/userPageTable.h"

extern struct PCB *threadA;

uint64_t read_file(char* file_name) {

    uint64_t* pml4_add = createUserProcess();
    threadA->mm = create_mm_struct();

    Elf64_Ehdr* eh = (Elf64_Ehdr*)read_tarfs(file_name);

    Elf64_Phdr* ph = (Elf64_Phdr*)((void*)eh + eh->e_phoff);

    updateUserCR3_Val((uint64_t)pml4_add);
    for (int i = 0; i < eh->e_phnum; ++i) {
        kprintf("%d\n",ph->p_type);
        if(ph->p_type == 1 && ph->p_offset == 0) {
           kprintf("%x\n",ph->p_vaddr);
           kprintf("%x\n",ph->p_paddr);
           kprintf("%d\n",ph->p_memsz);
           insert_vma(threadA->mm, ph->p_vaddr, ph->p_vaddr + ph->p_memsz, ph->p_memsz, ph->p_flags, ph->p_type);
           insert_stack_vma(threadA->mm, ph->p_vaddr+0x1000, ph->p_vaddr +0x2000, 0x1000, ph->p_flags, 10);
    	   //mapUserPageTable((uint64_t)pml4_add, ph->p_vaddr, ph->p_vaddr+ph->p_memsz, eh, ph->p_offset, ph->p_filesz);
	}
        ph += 1;
    }
    
    return eh->e_entry;
}

void mapUserPageTable(uint64_t pml4_add, uint64_t startAddress, uint64_t endAddress, Elf64_Ehdr* eh, uint64_t offset, uint64_t filesz)
{
	for(uint64_t i = startAddress; i <= endAddress; i += 0x1000)
	{
		walkUserPageTables(pml4_add, i);
		copyUserData(pml4_add, i,(uint64_t*) (eh+offset), filesz);
	}
}
