#include "sys/defs.h"
#include "sys/elf64.h"
#include "sys/tarfs.h"
#include "sys/kprintf.h"
#include "sys/pcb.h"
#include "sys/userPageTable.h"
#include "sys/process_manager.h"
#include "sys/thread.h"
#include "sys/stdarg.h"
#include "sys/utility.h"
#include "sys/string.h"

extern struct PCB* current_proc;


struct PCB* read_file(char* file_name, char *argv[], char *envp[]) {

    struct PCB *userThread = create_new_proc(file_name, 1); 
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

    loadCR3((uint64_t)pml4_add);
    
    for (int i = 0; i < eh->e_phnum; ++i)
    {
        if(ph->p_type == 1)
        {
           insert_vma(userThread->mm, ph->p_vaddr, ph->p_vaddr + ph->p_memsz, ph->p_memsz, ph->p_flags, ph->p_type);
    	   mapUserPageTable((uint64_t)pml4_add, ph->p_vaddr, ph->p_vaddr+ph->p_memsz, (uint64_t*)eh+(ph->p_offset), ph->p_filesz);
	}
        ph += 1;
    }
/*    uint64_t heapStartVAddress = H_BASE;
    uint64_t heapEndVAddress = H_END;
    insert_vma(userThread->mm, heapStartVAddress, heapStartVAddress + heapEndVAddress, heapStartVAddress-heapEndVAddress+1, 1, 20);
    struct PCBMemList* fp =  (struct PCBMemList*) kmalloc(sizeof(struct PCBMemList));
    fp->size = H_END;
    fp->baseAddress = H_BASE;
    fp->next = NULL;
    userThread->freeHeapList = fp;
*/ 
    uint64_t endStackVAddress = S_TOP;
    uint64_t startStackVAddress = S_TOP - S_SIZE;

    insert_vma(userThread->mm, startStackVAddress, endStackVAddress, endStackVAddress-startStackVAddress + 1, 1, 10);
    mapUserPageTable((uint64_t)pml4_add, endStackVAddress - 0x1000, endStackVAddress, (uint64_t*)(endStackVAddress - 0x1000), 0x1000);
    initializeProc(userThread, eh->e_entry, endStackVAddress-0x8);
    loadCR3(currentCR3);

    copyArgumentsToStack(file_name, userThread, argv, envp, (uint64_t*)(endStackVAddress-0x8));
    return userThread;
}

void mapUserPageTable(uint64_t pml4_add, uint64_t startAddress, uint64_t endAddress, uint64_t* offset, uint64_t filesz)
{
	for(uint64_t i = startAddress; i < endAddress; i += 0x1000)
	{
		walkUserPageTables(pml4_add, i, 0);
	}
    copyUserData(pml4_add, startAddress, offset, filesz);
}

uint64_t getArgCount(char *argv[])
{
  uint64_t cnt = 0;

  while(argv != NULL && argv[cnt] != NULL) {
    cnt += 1;
  }

  return cnt;
}

void copyArgumentsToStack(char* file_name, struct PCB* proc, char* argv[], char* envp[], uint64_t *user_stk) 
{
    char arg[15][50];
    char arge[15][50];
    uint64_t argc = getArgCount(argv);
    uint64_t envc = getArgCount(envp);
    uint64_t* argvadd[50];
    uint64_t* envpadd[50];

    if(file_name) {
       argc += 1;
       strcpy(arg[0], file_name);
    }
    
    int idx = 1;
    if (argv != NULL) {
        while (argv[idx-1]) {
            strcpy(arg[idx], argv[idx-1]);
            idx++;
        } 
    }

    if (envp != NULL) {
        while (envp[idx-1]) {
            strcpy(arge[idx], envp[idx-1]);
            idx++;
        } 
    }

    uint64_t currentCR3;
    
    __asm__ volatile
    (
        "movq %%cr3, %0 \n\t"
        :"=r" (currentCR3)
        :
        :"cc", "memory"
    );
    updateUserCR3_Val(proc->pml4);
    user_stk = (uint64_t*)proc->stop;
    
    for (int i = envc-1; i >= 0; i--) {
        user_stk = (uint64_t*)((void*)user_stk - (strlen(arge[i]) + 1));
        memcpy((char*)user_stk, arge[i], strlen(arge[i]) + 1);
        envpadd[i] = user_stk;
    }

    for (int i = argc-1; i >= 0; i--) {
        user_stk = (uint64_t*)((void*)user_stk - (strlen(arg[i]) + 1));
        memcpy((char*)user_stk, arg[i], strlen(arg[i]) + 1);
        argvadd[i] = user_stk;
    }


    for (int i = envc-1; i >= 0; i--) {
        user_stk--;
        *user_stk = (uint64_t)envpadd[i];
    }

    user_stk--;
    *user_stk = 0x0;
    
    for (int i = argc-1; i >= 0; i--) {
        user_stk--;
        *user_stk = (uint64_t)argvadd[i];
    }

    // user_stk--;
    // *user_stk = (uint64_t)envc;

    user_stk--;
    *user_stk = (uint64_t)argc;
    
    // __asm__ volatile
    // (
    //     "movq %0, %%rsp \n\t"
    //     :
    //     :"a" (user_stk)
    //     :"cc", "memory"
    // );
    proc->stop = (uint64_t)user_stk;
    updateUserCR3_Val(currentCR3);

}
