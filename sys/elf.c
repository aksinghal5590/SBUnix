#include "sys/elf64.h"
#include "sys/tarfs.h"
#include "sys/kprintf.h"
//#include "sys/defs.h"
#include "sys/pcb.h"

extern struct PCB *threadA;

uint64_t read_file(char* file_name) {

    threadA->mm = create_mm_struct();

    Elf64_Ehdr* eh = (Elf64_Ehdr*)read_tarfs(file_name);

    Elf64_Phdr* ph = (Elf64_Phdr*)((void*)eh + eh->e_phoff);

    uint64_t ret = 0;
    for (int i = 0; i < eh->e_phnum; ++i) {
        kprintf("%d\n",ph->p_type);
        if(ph->p_type == 1 && ph->p_offset == 0) {
           kprintf("%x\n",ph->p_vaddr);
           kprintf("%x\n",ph->p_paddr);
           kprintf("%d\n",ph->p_memsz);
           ret = ph->p_vaddr;
           insert_vma(threadA->mm, ph->p_vaddr, ph->p_vaddr + ph->p_memsz, ph->p_memsz, ph->p_flags, ph->p_type);
        }
        ph += 1;
    }

    return ret;
}
