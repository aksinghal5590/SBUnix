
#include "sys/pcb.h"
#include "sys/thread.h"
#include "sys/process_manager.h"
#include "sys/thread.h"
#include "sys/kprintf.h"
struct PCB* current_proc = NULL;
extern void irq0();
void idle_process(){
	while(1);
}

void init_idle_process(){
	struct PCB* idle = create_new_proc("idle_task"); 
	idle->state = IDLE;
	schedule_proc(idle, (uint64_t)idle_process, (uint64_t)&idle->kstack[KSTACK_SIZE-1]);
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
    // 1) Set up kernel stack => ss, rsp, rflags, cs, rip
    // if (new_task->IsUserProcess) {
    //     new_task->kernel_stack[KERNEL_STACK_SIZE-1] = 0x23;
    //     new_task->kernel_stack[KERNEL_STACK_SIZE-4] = 0x1b;
    // } else {
    //     new_task->kernel_stack[KERNEL_STACK_SIZE-1] = 0x10;
    //     new_task->kernel_stack[KERNEL_STACK_SIZE-4] = 0x08;
    //  }
    kprintf("%s\n", "asmksds");
    proc->kstack[KSTACK_SIZE-1] = 0x23;
    proc->kstack[KSTACK_SIZE-4] = 0x2b;
    proc->kstack[KSTACK_SIZE-2] = stop;
    proc->kstack[KSTACK_SIZE-3] = 0x200202UL;
    proc->kstack[KSTACK_SIZE-5] = entry;

    // proc->rip = entry;

// #if DEBUG_SCHEDULING
//     kprintf("\tEntry Point:%p", entry_point);
//     kprintf("\tStackTop:%p", stack_top);
// #endif

    // 2) Leave 9 spaces for POPA => KERNEL_STACK_SIZE-6 to KERNEL_STACK_SIZE-20

    // 3) Set return address to POPA in irq0()
    proc->kstack[KSTACK_SIZE-21] = (uint64_t)irq0 + 0x20;

    // 4) Set rsp to KERNEL_STACK_SIZE-16
    proc->rsp = (uint64_t)&proc->kstack[KSTACK_SIZE-22];
    proc->rip = entry;

    // 5) Add to the next_task_list 
    add_proc_to_list(proc);
    print_task_list();

}

/*void print_task_list()
{
	struct PCB *t = task_l;
	while(t != NULL)
	{
		kprintf("%d	%d	%s	 %s \n",t->pid,t->ppid,t->p_name,t->state);
 		kprintf2("task id %d", t->pid);
		kprintf2("task name %s", t->name);
		kprintf2("task state %d \n", t->state);
		t = t->next;
	}
}*/

/*void copyProcess(struct PCB* parent) {

    struct PCB* child  = createThread();
    uint64_t parent_pml4   = parent->pml4;
    uint64_t child_pml4    = child->pml4;
    
    vm_area_struct *parent_vma = parent->mm->vma_list;
    vm_area_struct *child_vma  = NULL;

    memcpy((void*)child->mm, (void*)parent->mm, sizeof(mm_struct));
    child->pml4  = child_pml4;
    child->mm->vma_list = NULL; 

    child->ppid  = parent->pid;
    // child_->parent = parent_task;
    kstrcpy(child_task->comm, parent_task->comm);

    // add_child_to_parent(child_task);
    while(parent_vma != NULL) {
    	uint64_t start, end , vadd, padd;
        // uint64_t *pte_entry, page_flags;

        vm_start = parent_vma->start;
        vm_end   = parent_vma->end;  

        if (child->mm->vma_list == NULL) {
            child_vma = child->mm->vma_list = create_vm_area_struct(start, end, parent_vma->access_flags, parent_vma->type);
        } else {
            child_vma->next = create_vm_area_struct(start, end, parent_vma->access_flags, parent_vma->type);
            child_vma = child_vma->next;
        }

        if (parent_vma->type == STK) {

            v_add = ((vm_end) >> 12 << 12) - 0x1000;
            while (v_add >= vm_start) {
                updateUserCR3_Val(parent_pml4);

                // pte_entry = get_pte_entry(vaddr);
                // if (!IS_PRESENT_PAGE(*pte_entry))
                //     break;
                if (!user_page_exists(parent_pml4, v_add)) {
                	break;
                }
                // Allocate a new page in kernel
                uint64_t ker_vadd = kmalloc(sizeof(struct PCB));
                p_add = ker_vadd - kernmem;

                //kprintf("\nStack v:%p p:%p", vaddr, paddr);
                // Copy parent page in kernel space
                memcpy((void*)ker_vadd, (void*)v_add, PAGESIZE);

                // Map paddr with child vaddr
                updateUserCR3_Val(child_pml4);
                //kprintf("\nStack v:%p p:%p", vaddr, paddr);
                map_virt_to_phys(v_add, p_add);

                *(get_page_entry_ptr(ker_vadd)) = 0UL;

                v_add = v_add - PAGESIZE;
            }

        } else {
        	v_add = ((vm_start) >> 12 << 12); //align page
            while (v_add < vm_end) {
                updateUserCR3_Val(parent_pml4);

                uint64_t* page_entry = get_page_entry_ptr(v_add);

                if (user_page_exists(parent_pml4, v_add)) {
                    *page_entry= *page_entry & 0xFFFFFFFFFFFFFFFDUL; // resret write bit
                    *page_entry = *page_entry | 0x4000000000000000UL; // set cow bit
                    // page_flags = *pte_entry & PAGING_FLAGS;

                    updateUserCR3_Val(child_pml4);
 
                    map_virt_to_phys(v_add, *page_entry);
                    // phys_inc_block_ref(paddr);
                }
                v_add = v_add + PAGESIZE;
            }
        }
        updateUserCR3_Val(parent_pml4);
        parent_vma = parent_vma->next;
    }

}*/
