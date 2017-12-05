#include "sys/pcb.h"
#include "sys/thread.h"
#include "sys/process_manager.h"
#include "sys/thread.h"
#include "sys/kprintf.h"
#include "sys/string.h"
#include "sys/userPageTable.h"
#include "sys/kernelLoad.h"
#include "sys/utility.h"

#define STK 10

extern struct PCB* current_proc;
extern void irq0();
extern char kernmem;
extern struct PCB* task_l;
extern struct PCB* idle;

extern void schedule(uint64_t* firstProc, uint64_t* secondProc);
extern void set_tss_rsp(void* rsp);
extern int check_proc_present();

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
        :"cc", "memory"
    );
    set_tss_rsp(&current_proc->kstack[99]);
    __asm__ volatile("iretq");
    while(1)
    {
        kprintf("Hello\n");
    }
}

void idle_process(){
    uint64_t i = 0;	
    while(1)
    {
        i++;
        if(i < 4)
        {
            kprintf("In idle task\n");
            if(check_proc_present())
            {
                struct PCB* t = get_next_proc();
                loadCR3(t->pml4);
                current_proc = t;
                schedule(&idle->rsp, &t->rsp);
            }
        }
    }
}

void init_idle_process(){
	idle = create_new_proc("idle_task", 0); 
	idle->state = IDLE;
	schedule_proc(idle, (uint64_t)&idle_process, (uint64_t)&idle->kstack[KSTACK_SIZE-1]);
}

// struct PCB* get_current_task()
// {
// 	return current_task;	
// }

// void set_current_task(struct PCB* task)
// {
// 	current_task = task;
// }

void schedule_proc(struct PCB* proc, uint64_t entry, uint64_t stop)
{
    kprintf("%s\n", "Scheduling a process.");
    if(!proc->isUser)
    {
        proc->kstack[KSTACK_SIZE-1] = 0x10;
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
    add_proc_to_list(proc);
    print_task_list();
}

/*void print_task_list()
{
	{
		kprintf("%d	%d	%s	 %s \n",t->pid,t->ppid,t->p_name,t->state);
 		kprintf2("task id %d", t->pid);
		kprintf2("task name %s", t->name);
		kprintf2("task state %d \n", t->state);
		t = t->next;
	}
}*/

void copyProcess(struct PCB* parent) {

    struct PCB* child  = createThread();
    uint64_t parent_pml4   = parent->pml4;
    uint64_t child_pml4    = child->pml4;
    
    struct vm_area_struct *parent_vma = parent->mm->vma_list;
    struct vm_area_struct *child_vma  = NULL;

    memcpy((void*)child->mm, (void*)parent->mm, sizeof(struct mm_struct));
    child->pml4  = child_pml4;
    child->mm->vma_list = NULL; 

    child->ppid  = parent->pid;
    // child_->parent = parent_task;
    strcpy(child->p_name, parent->p_name);

    // add_child_to_parent(child_task);
    while(parent_vma != NULL) {
    	uint64_t vm_start, vm_end , v_add, p_add;
        // uint64_t *pte_entry, page_flags;
        vm_start = parent_vma->start;
        vm_end   = parent_vma->end;  

        if (child->mm->vma_list == NULL) {
            child_vma = child->mm->vma_list = create_vm_area_struct(vm_start, vm_end, vm_end-vm_start+1, parent_vma->access_flags, parent_vma->type);
        } else {
            child_vma->next = create_vm_area_struct(vm_start,vm_end, vm_end-vm_start+1,parent_vma->access_flags, parent_vma->type);
            child_vma = child_vma->next;
        }

        if (parent_vma->type == STK) {

            v_add = ((vm_end) >> 12 << 12) - 0x1000;
            while (v_add >= vm_start) {
                updateUserCR3_Val(parent_pml4);

                // pte_entry = get_pte_entry(vaddr);
                // if (!IS_PRESENT_PAGE(*pte_entry))
                //     break;
                if (!user_page_exist(parent_pml4, v_add)) {
                	break;
                }
                // Allocate a new page in kernel
                uint64_t ker_vadd = (uint64_t)kmalloc(sizeof(struct PCB));
                p_add = ker_vadd - kernmem;

                //kprintf("\nStack v:%p p:%p", vaddr, paddr);
                // Copy parent page in kernel space
                memcpy((void*)ker_vadd, (void*)v_add, PAGESIZE);

                // Map paddr with child vaddr
                updateUserCR3_Val(child_pml4);
                //kprintf("\nStack v:%p p:%p", vaddr, paddr);
                useExistingPage(child_pml4, v_add, p_add);

                *(getPTTableEntry(child_pml4, ker_vadd)) = 0UL;

                v_add = v_add - PAGESIZE;
            }

        } else {
        	v_add = ((vm_start) >> 12 << 12); //align page
            while (v_add < vm_end) {
                updateUserCR3_Val(parent_pml4);

                uint64_t* page_entry = getPTTableEntry(parent_pml4, v_add);

                if (user_page_exist(parent_pml4, v_add)) {
                    *page_entry= *page_entry & 0xFFFFFFFFFFFFFFFDUL; // resret write bit
                    *page_entry = *page_entry | 0x4000000000000000UL; // set cow bit
                    // page_flags = *pte_entry & PAGING_FLAGS;

                    updateUserCR3_Val(child_pml4);
 
                    useExistingPage(child_pml4, v_add, *page_entry);
                    // phys_inc_block_ref(paddr);
                }
                v_add = v_add + PAGESIZE;
            }
        }
        updateUserCR3_Val(parent_pml4);
        parent_vma = parent_vma->next;
    }

}

void schedule_next_process()
{
    struct PCB* temp = current_proc;
    struct PCB* next_proc = get_next_proc();
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
        add_proc_to_list(temp);
    }
    current_proc = next_proc;
    schedule(&temp->rsp, &current_proc->rsp);
}
