#include "sys/elf64.h"
#include "sys/tarfs.h"
#include "sys/kprintf.h"
//#include "sys/defs.h"

void read_file(char* file_name) {
    Elf64_Ehdr* eh = (Elf64_Ehdr*)read_tarfs(file_name);

    Elf64_Phdr* ph = (Elf64_Phdr*)((void*)eh + eh->e_phoff);


    for (int i = 0; i < eh->e_phnum; ++i) {
        kprintf("%d\n",ph->p_type);
        if(ph->p_type == 1) {
           kprintf("%x\n",ph->p_vaddr);
           kprintf("%x\n",ph->p_paddr);
           kprintf("%d\n",ph->p_memsz);
        }
        ph += 1;
    }

}
