#include "sys/pcb.h"
#include "sys/thread.h"
#include "sys/process_manager.h"
#include "sys/thread.h"
#include "sys/kprintf.h"
#include "sys/string.h"
#include "sys/userPageTable.h"
#include "sys/utility.h"

#define STK 10
#define HEAP 20
#define H_BASE 0xF0000000
#define H_END  0x100000

struct PCB *ready_proc_list = NULL;
struct PCB *sleep_proc_list = NULL;
struct PCB* current_proc;
extern struct PCB* idle ;
extern struct PCB* init_proc ;

extern char kernmem;
extern void schedule(uint64_t* firstProc, uint64_t* secondProc);
extern void set_tss_rsp(void* rsp);

extern int check_proc_present();
void idle_process();

void switch_to_ring3_from_kernel()
{
    uint64_t s = current_proc->stop;
    uint64_t* stack = (uint64_t*)s;
    uint64_t e = current_proc->rip;
    __asm__ volatile
    (
        "pushq $0x23;"
        "pushq %[stack];"
        "pushfq;"
        "pushq $0x2B;"
        "pushq %[e];"
        :
        :[stack]"g"(stack), [e]"g"(e)
        :"cc", "memory", "rax"
    );
    set_tss_rsp(&current_proc->kstack[KSTACK_SIZE-1]);
    __asm__ volatile("iretq");
}

void initIdleProcess(){
	idle = create_new_proc("idle_task", 0); 
	idle->state = IDLE;
	initializeProc(idle, (uint64_t)&idleProcess, (uint64_t)&idle->kstack[KSTACK_SIZE-1]);
}

void idleProcess(){
    
    while(1)
    { 
            // kprintf("In idle task\n");
            if(checkReadyProcPresent())
            {
                struct PCB* t = getNextReadyProc();
                loadCR3(t->pml4);
                current_proc = t;
                current_proc->state = RUNNING;
                schedule(&idle->rsp, &t->rsp);
            }
    }
}

void initializeProc(struct PCB* proc, uint64_t entry, uint64_t stop)
{

    if(!proc->isUser)
    {
        proc->kstack[KSTACK_SIZE-1] = 0x10; //kernel segment
        proc->kstack[KSTACK_SIZE-4] = 0x08;    
        proc->kstack[KSTACK_SIZE-2] = stop;
        proc->kstack[KSTACK_SIZE-3] = 0x200202UL;
        proc->kstack[KSTACK_SIZE-5] = entry;
    }
    else
    {
        proc->kstack[KSTACK_SIZE-1] = (uint64_t)&switch_to_ring3_from_kernel;
    }
    
    if(proc->isUser)
        proc->rsp = (uint64_t)&proc->kstack[KSTACK_SIZE-17];
    else
        proc->rsp = (uint64_t)&proc->kstack[KSTACK_SIZE-5];

    proc->rip = entry;
    proc->stop = stop;
    proc->usedHeapList = NULL;
    addProcToReadyList(proc);
}

struct PCB* copyProcess(struct PCB* parent) {

    struct PCB* child  = create_new_proc("child", 1);

    uint64_t parent_pml4   = parent->pml4;
    uint64_t child_pml4    = child->pml4;
    
    struct vm_area_struct *parent_vma = parent->mm->vma_list;
    struct vm_area_struct *child_vma  = NULL;

    memcpy((void*)child->mm, (void*)parent->mm, sizeof(struct mm_struct));
    child->pml4  = child_pml4;
    child->mm->vma_list = NULL; 

    child->ppid  = parent->pid;
    strcpy(child->p_name, parent->p_name);
    parent->child_list[child->pid] = 1;
    while(parent_vma != NULL) {
    	uint64_t vm_start, vm_end , v_add, p_add;
        vm_start = parent_vma->start;
        vm_end   = parent_vma->end;  

        if (child->mm->vma_list == NULL) {
            child_vma = child->mm->vma_list = create_vm_area_struct(vm_start, vm_end, vm_end-vm_start+1, parent_vma->access_flags, parent_vma->type);
        } else {
            child_vma->next = create_vm_area_struct(vm_start,vm_end, vm_end-vm_start+1,parent_vma->access_flags, parent_vma->type);
            child_vma = child_vma->next;
        }

        if (parent_vma->type == STK) {

            // v_add = ((vm_end) >> 12 << 12) - 0x1000;
            v_add = vm_end - 0x1000;
            while (v_add >= vm_start) {
                updateUserCR3_Val(parent_pml4);
                if (!user_page_exist(parent_pml4, v_add)) {
                	break;
                }
                uint64_t ker_vadd = (uint64_t)kmalloc(sizeof(struct PCB));

                p_add = ker_vadd - VIRTUAL_BASE;

                //kprintf("\nStack v:%p p:%p", vaddr, paddr);
                // Copy parent page in kernel space
                memcpy((void*)ker_vadd, (void*)v_add, PAGESIZE);
                updateUserCR3_Val(child_pml4);
                useExistingPage(child_pml4, v_add, p_add);
                *(getPTTableEntry(child_pml4, ker_vadd)) = 0UL;
                v_add = v_add - PAGESIZE;
            }
        } 
        else {
        	// v_add = ((vm_start) >> 12 << 12); //align page
            v_add = vm_start;
            while (v_add < vm_end) {
                updateUserCR3_Val(parent_pml4);
                if(parent_vma->type == HEAP && (!user_page_exist(parent_pml4, v_add)))
			break;


                if (user_page_exist(parent_pml4, v_add)) {
		    uint64_t* page_entry = getPTTableEntry(parent_pml4, v_add);
                    *page_entry= *page_entry & 0xFFFFFFFFFFFFFFFDUL;
                    *page_entry = *page_entry | 0x4000000000000000UL;
                    updateUserCR3_Val(child_pml4);
                    useExistingPage(child_pml4, v_add, *page_entry);
                }
                v_add = v_add + PAGESIZE;
            }
        }
        updateUserCR3_Val(parent_pml4);
        parent_vma = parent_vma->next;
    }
    return child;
}

void loadNextProcess()
{
    struct PCB* temp = current_proc;
    struct PCB* next_proc = getNextReadyProc();
    
    if(next_proc == NULL)
        next_proc = idle;
    
    loadCR3(next_proc->pml4);

    if(temp->state == EXIT)
    {
        temp->next = NULL;
        //to do
    }
    else
    {
        addProcToReadyList(temp);
    }
    current_proc = next_proc;
    current_proc->state = RUNNING;
    schedule(&temp->rsp, &current_proc->rsp);
}

void addProcToReadyList(struct PCB* proc)
{
    if(proc->state == IDLE)
        return;
    else if(proc->state == RUNNING || proc->state == SLEEPING)
        proc->state = READY;
    
    if(ready_proc_list == NULL)
    {
        ready_proc_list = proc;
    }
    else
    {
        struct PCB* temp = ready_proc_list;
        while(temp->next != NULL)
        {
            temp = temp->next;
        }
        temp->next = proc;
        proc->next = NULL;
    }
}

void addProcToSleepList(struct PCB* proc)
{
    if(proc->state == IDLE)
        return;
    else if(proc->state == RUNNING)
        proc->state = SLEEPING;
    
    if(sleep_proc_list == NULL)
    {
        sleep_proc_list = proc;
    }
    else
    {
        struct PCB* temp = sleep_proc_list;
        while(temp->next != NULL)
        {
            temp = temp->next;
        }
        temp->next = proc;
        proc->next = NULL;
    }
}

struct PCB* getNextReadyProc() {
	
    if(ready_proc_list == NULL)
        return NULL;
    else
    {
        struct PCB* proc = ready_proc_list;
        ready_proc_list = ready_proc_list->next;
        proc->next = NULL;
        return proc;
    }
}

struct PCB* getNextSleepProc()
{	
    if(sleep_proc_list == NULL)
        return NULL;
    else
    {
        struct PCB* proc = sleep_proc_list;
        sleep_proc_list = sleep_proc_list->next;
        proc->next = NULL;
        return proc;
    }
}

int checkReadyProcPresent()
{
    if(ready_proc_list != NULL)
        return 1;
    return 0;
}

void printReadyList()
{
        struct PCB *t = ready_proc_list;
        while(t != NULL)
        {
                // kprintf("pid: %d     ppid: %d\n",t->pid,t->ppid);
                // kprintf("task name %s\n", t->p_name);
                // kprintf("task state %d \n", t->state);
                t = t->next;
        }
}

void addToFrontReady(struct PCB* proc) {
    if(proc->state == IDLE)
        return;
    else if(proc->state == RUNNING || proc->state == READY)
    {
        proc->state = READY;
    }
    else
        return;

    if(ready_proc_list == NULL)
    {
        ready_proc_list = proc;
    }
    else
    {
        proc->next = ready_proc_list;
        ready_proc_list = proc;
    }
}

void printSleepList()
{
        struct PCB *t = sleep_proc_list;
        while(t != NULL)
        {
                // kprintf("pid: %d     ppid: %d\n",t->pid,t->ppid);
                // kprintf("task name %s", t->p_name);
                // kprintf("task state %d \n", t->state);
                t = t->next;
        }
}
