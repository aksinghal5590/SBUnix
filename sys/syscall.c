#include "sys/defs.h"
#include "sys/syscall.h"
#include "sys/kprintf.h"
#include "sys/process_manager.h"
#include "sys/pcb.h"
#include "sys/elf64.h"

extern struct PCB* current_proc;

extern void add_proc_to_list(struct PCB* proc);
extern void writeSyscall(uint64_t fd, uint64_t data, uint64_t len, uint64_t sysNum);
extern void sysCallHandler();

extern pid_t forkSyscall(uint64_t sysNum);

extern void schedule_next_process();
extern void getCharacters(uint64_t data, uint64_t len);

uint64_t* function_ptr = NULL;

extern struct PCB* current_proc;
void* systemCallHandlerTable[7] = {systemRead, 
	systemWrite, systemExit, systemYield, 
	systemFork, systemExecvpe, systemWaitPid}; 


void userWrite(uint64_t fileDescriptor, char* data, uint64_t len)
{
	 writeSyscall(fileDescriptor, (uint64_t)data, len, 1);
}


pid_t userFork()
{
	 return forkSyscall(4);
}
/*void systemCallHandler()
{
    sysCallHandler();
    //__asm__ volatile("iretq");
}*/

void systemCallHandler()
{
    uint64_t sysNum;
	__asm__ volatile
	(
		"pushq %rdx;"
	);
	__asm__ volatile
	(
		"movq %%rax, %0;"
		: "=r"(sysNum)
		:
		: "cc", "memory"
	);
	if(sysNum >= 0)
	{
		function_ptr = systemCallHandlerTable[sysNum];
		__asm__ volatile
		(
			"popq %%rdx;"
			"callq %0;"
			:
			: "r" (function_ptr)
			: "cc", "rcx", "memory"	
		);
	}
    __asm__ volatile
    (
        "iretq;"
        :
        :
        :"cc", "memory"
    );
}

void systemWrite(uint64_t fd, uint64_t data, uint64_t len)
{
	char* d = (char*)data;
	kprintf("Data is: %s\n", d);
	/*for(uint64_t i = 0; i < len; i++)
	{
		kprintf("%c", d++);
	}*/
}

void systemRead(uint64_t fileDescriptor, uint64_t data, uint64_t len)
{
    if(fileDescriptor==0)
    {
        if(len==0)
            return;
        getCharacters(data, len);
    }
}


pid_t systemFork()
{
    struct PCB *parent = current_proc; 
    struct PCB *child = copyProcess(parent); 
    schedule_proc(child, parent->kstack[KSTACK_SIZE-7], parent->kstack[KSTACK_SIZE-4]);
    //child->rsp = 0xffffffff802f0230;
    child->kstack[KSTACK_SIZE-2] = 0UL;
    kprintf("%d\n", child->pid);
    print_task_list();
    return child->pid;
}

void systemExit(uint64_t status)
{
    kprintf("Process exit with status: %d\n", status);
    current_proc->state = EXIT;
    schedule_next_process();
}

void systemYield()
{
    kprintf("Inside Yield\n");
    current_proc->state = READY;
    //add_proc_to_list(current_proc);
    schedule_next_process();
}


//TODO copy siblings and child
int systemExecvpe(char *file_path, char *argv[], char *envp[])
{
    struct PCB *exec = read_file(file_path, (uint64_t*)argv , (uint64_t*)envp);
    kprintf("%s\n", "Inside Execvpe");

    if (exec != NULL) {
        // struct PCB *temp = current_proc;

        // Exec process has same pid, ppid and parent
        set_pid(exec->pid);
        exec->pid  = current_proc->pid;
        exec->ppid = current_proc->ppid;
        exec->parent = current_proc->parent;
        //memcpy((void*)new_task->file_descp, (void*)cur_task->file_descp, MAXFD*8);

        // // Replace current child with new exec process
        // replace_child_task(cur_task, new_task);

        // Exit from the current process
        // empty_task_struct(cur_task);
        current_proc->state = EXIT;
        // schedule_next_process()
        systemExit(0);
 
    }
    // execvpe failed; so return -1
    return -1;
}

//TODO check if valid pid
uint64_t systemWaitPid(uint64_t pid, uint64_t status, uint64_t options)
{
    // int *status_p = (int*) fstatus;
    if (current_proc->child_cnt == 0) {
        //if (status_p) *status_p = -1;
        return -1;
    }

    if (pid > 0) {
        current_proc->wait_on_child_pid = pid;
    } else {
        current_proc->wait_on_child_pid = 0;
    }

    current_proc->state = WAIT;

    // if (status_p) *status_p = 0;
    return (uint64_t)current_proc->wait_on_child_pid;
}
