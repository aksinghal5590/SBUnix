#include <sys/pcb.h>
#include <sys/kernelLoad.h>



struct PCB *task_l = NULL;
struct PCB *free_task_l = NULL;
static uint64_t pid = 1;
struct PCB *current_proc = NULL;

struct mm_struct* create_mm_struct() {
	struct mm_struct *mm = (struct mm_struct*) kmalloc(sizeof(struct mm_struct));
	mm->vma_list = NULL;
	return mm;
}

struct vm_area_struct* create_vm_area_struct(uint64_t start, uint64_t end, uint64_t size, uint64_t access_flags, uint64_t type) {

	struct vm_area_struct *vma = (struct vm_area_struct*) kmalloc(sizeof(struct vm_area_struct));
	vma->start = start;
	vma->end = end;
	vma->size = size;
	vma->access_flags = access_flags;
	vma->type = type;
	vma->prev = NULL;
	vma->next = NULL;
	return vma;
}

void insert_vma(struct mm_struct *mm, uint64_t start, uint64_t end, uint64_t size, uint64_t access_flags, uint64_t type) {

	struct vm_area_struct *new_vma = create_vm_area_struct(start, end, size, access_flags, type);
	struct vm_area_struct *temp = mm->vma_list;
	int added = 0;
	if(mm->vma_list == NULL) {
		mm->vma_list = new_vma;
		return;
	}
	while(temp->next != NULL) {
		if(temp->start < end) {
			if(temp->end <= start) {
				new_vma->prev = temp->prev;
				new_vma->next = temp;
				temp->prev->next = new_vma;
				temp->prev = new_vma;
			} else {
				//TODO
				//break existing into 2 and insert new_vma in between
			}
			added = 1;
			break;
		}
		temp = temp->next;
	}
	if(added == 0) {
		temp->next = new_vma;
		new_vma->prev = temp;
	}
}


struct PCB *create_new_proc(char *p_name)
{
	// struct PCB *proc = get_free_task_struct();

	// if(strcmp(p_name, "idle_proc") != 0)
	// 	add_proc_to_list(proc);
	struct PCB *proc =  ((struct PCB*) kmalloc(sizeof(struct PCB)));
	proc->pid = pid++;
	proc->ppid = 0;
	// new_task->sleep_time = 0;
	proc->parent = NULL;
	// new_task->pipe = NULL;
	proc->child_count = 0;

	// new_task->write_redirection_fd = 0;
	// new_task->read_redirection_fd = 0;
	proc->wait_on_child_pid = 0;
	strcpy(proc->p_name, p_name);
	proc->mm = create_new_mmstruct();
	proc->state = READY;
	proc->pml4e = createUserPML4Table();	//virtual address
	memset((void*)proc->kstack, 0, KSTACK_SIZE);
	proc->next = NULL;
	// new_task->r.ds = task->r.fs = task->r.es = task->r.gs = 0x23;
	
	//kprintf2("task pid is %d \n", task->pid);
	return proc;
}


struct PCB* get_free_task_struct(){
	struct PCB *proc = NULL;
	
	if(free_task_l != NULL){ 
		proc = free_task_l;
		free_task_l = free_task_l->next;
	}	

	return proc;
	// proc = (struct PCB *)kmalloc(sizeof(struct PCB));
	// proc->mm = create_mm_struct(); 
	// return proc; 
}

struct PCB* get_next_proc() {

	if(!current_proc)
	{	
		struct PCB *proc = current_proc->next;
		while(proc)
		{
			if(proc->state == READY)
				return proc;
			proc = proc->next;
		}
		proc = task_l;

	} else
		return task_l;
	
	return NULL;
}



void add_proc_to_list(struct PCB* proc)
{
    PCB* ptr = task_l;

    if (proc->state == IDLE) {
        return;
    } else if (proc->state == RUNNING) {
        proc->state = READY;
    }

    if (ptr == NULL) {
        task_l = proc;
    } else {
        while (ptr->next != NULL) {
            ptr = ptr->next;
        }
        
        ptr->next = proc;
        
        proc->next = NULL;
    }
}