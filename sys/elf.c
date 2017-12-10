#include "sys/defs.h"
#include "sys/elf64.h"
#include "sys/tarfs.h"
#include "sys/kprintf.h"
#include "sys/pcb.h"
#include "sys/userPageTable.h"
#include "sys/process_manager.h"
#include "sys/thread.h"
#include "sys/stdarg.h"
#include "sys/string.h"

#define S_TOP 0xF0000000
#define S_SIZE 0x10000

extern struct PCB* current_proc;

// struct PCB *userThread;

struct PCB* read_file(char* file_name, uint64_t *argv[], uint64_t *envp[]) {

    struct PCB *userThread = create_new_proc("User Process", 1); 
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

    updateUserCR3_Val((uint64_t)pml4_add);
    for (int i = 0; i < eh->e_phnum; ++i) {
        kprintf("%d\n",ph->p_type);
        if(ph->p_type == 1 && ph->p_offset == 0) {
           kprintf("%x\n",ph->p_vaddr);
           kprintf("%x\n",ph->p_paddr);
           kprintf("%d\n",ph->p_memsz);

           insert_vma(userThread->mm, ph->p_vaddr, ph->p_vaddr + ph->p_memsz, ph->p_memsz, ph->p_flags, ph->p_type);
           //insert_vma(threadA->mm, ph->p_vaddr+0x1000, ph->p_vaddr +0x2000, 0x1000, ph->p_flags, 10);
    	   //mapUserPageTable((uint64_t)pml4_add, ph->p_vaddr, ph->p_vaddr+ph->p_memsz, eh, ph->p_offset, ph->p_filesz);
           //insert_vma(userThread->mm, ph->p_vaddr, ph->p_vaddr + ph->p_memsz, ph->p_memsz, ph->p_flags, ph->p_type);
    	    mapUserPageTable((uint64_t)pml4_add, ph->p_vaddr, ph->p_vaddr+ph->p_memsz, (uint64_t*)eh+(ph->p_offset), ph->p_filesz);

	      }
           //insert_vma(threadA->mm, ph->p_vaddr+0x1000, ph->p_vaddr +0x2000, 0x1000, ph->p_flags, 10);
    	   //mapUserPageTable((uint64_t)pml4_add, ph->p_vaddr, ph->p_vaddr+ph->p_memsz, eh, ph->p_offset, ph->p_filesz);
        ph += 1;
    }
    struct vm_area_struct *temp = userThread->mm->vma_list;
    while(temp->next != NULL)
    {
        temp = temp->next;
    }
    uint64_t endStackVAddress = S_TOP;
    uint64_t startStackVAddress = S_TOP - S_SIZE;
    insert_vma(userThread->mm, startStackVAddress, endStackVAddress, endStackVAddress-startStackVAddress+1, 1, 10);
    mapUserPageTable((uint64_t)pml4_add, endStackVAddress-0x1000, endStackVAddress, (uint64_t*)(endStackVAddress-0x1000), 0x1000);
    updateUserCR3_Val(currentCR3);
    //TODO Copy argument to stacks
    copyArgumentsToStack(file_name, userThread, argv, envp, (uint64_t*)endStackVAddress-0x8);
    schedule_proc(userThread, eh->e_entry, endStackVAddress-0x8);
    return userThread;
}

void mapUserPageTable(uint64_t pml4_add, uint64_t startAddress, uint64_t endAddress, uint64_t* offset, uint64_t filesz)
{
	for(uint64_t i = startAddress; i < endAddress; i += 0x1000)
	{

		walkUserPageTables(pml4_add, i, 0);
    copyUserData(pml4_add, i,offset, filesz);

	}
}

uint64_t getArgCount(uint64_t *argv[])
{
  uint64_t cnt = 0;
  while(argv[cnt]) {
    cnt += 1;
  }
  
  return cnt;
}

void copyArgumentsToStack(uint64_t* file_name, struct PCB* proc, uint64_t* argv[], uint64_t* envp[], uint64_t *user_stk) 
{
    char arg[15][50];
    char arge[15][50];
    uint64_t argc = getArgCount(argv);
    uint64_t envc = getArgCount(envp);

    if(file_name) {
       argc += 1;
       strcpy(arg[0], file_name);
    }
    
    int idx = 1;
    if (argv != NULL) {
        while (argv[idx-1]) {
            strcpy(arg[idx], (void*)argv[idx-1]);
            idx++;
        } 
    }

    if (envp != NULL) {
        while (envp[idx-1]) {
            strcpy(arge[idx], (void*)envp[idx-1]);
            idx++;
        } 
    }
    
    updateUserCR3_Val(proc->pml4);

    for (int i = envc-1; i >= 0; i--) {
        user_stk = (uint64_t*)((void*)user_stk - (strlen(arge[i]) + 1));
        memcpy((char*)user_stk, arge[i], strlen(arge[i]) + 1);
        envp[i] = user_stk;
    }

    for (int i = argc-1; i >= 0; i--) {
        user_stk = (uint64_t*)((void*)user_stk - (strlen(arg[i]) + 1));
        memcpy((char*)user_stk, arg[i], strlen(arg[i]) + 1);
        argv[i] = user_stk;
    }


    for (int i = envc-1; i >= 0; i--) {
        user_stk--;
        *user_stk = (uint64_t)envp[i];
    }

    user_stk--;
    *user_stk = 0x0;
    
    for (int i = argc-1; i >= 0; i--) {
        user_stk--;
        *user_stk = (uint64_t)argv[i];
    }

    // user_stk--;
    // *user_stk = (uint64_t)envc;

    user_stk--;
    *user_stk = (uint64_t)argc;

    updateUserCR3_Val(current_proc->pml4);

}
