#include <sys/pcb.h>
#include <sys/kernelLoad.h>
#include <sys/string.h>
#include  "sys/userPageTable.h"
#include <sys/memset.h>
#include <sys/kprintf.h>

struct PCB *task_l = NULL;
struct PCB *free_task_l = NULL;
struct PCB* current_proc = NULL;
struct PCB* idle = NULL;
static uint64_t pid = 0;
extern struct PCB* current_proc;

//struct PCB *current_proc = NULL;
void set_pid(pid_t curr_pid) {
	pid = curr_pid;
}

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

struct PCB *create_new_proc(char *p_name, uint8_t isUser)
{
	// struct PCB *proc = get_free_task_struct();

	// if(strcmp(p_name, "idle_proc") != 0)
	// 	add_proc_to_list(proc);
	struct PCB *proc =  ((struct PCB*) kmalloc(sizeof(struct PCB)));
	proc->pid = pid++;
	kprintf("PID of %s - %d\n", p_name, proc->pid);
	proc->ppid = 0;
	// new_task->sleep_time = 0;
	proc->parent = NULL;
	// new_task->pipe = NULL;
	proc->child_cnt = 0;

	// new_task->write_redirection_fd = 0;
	// new_task->read_redirection_fd = 0;
    proc->isUser = isUser;
	proc->wait_on_child_pid = 0;
	strcpy(proc->p_name, p_name);
	proc->mm = create_mm_struct();
	proc->state = READY;
	proc->pml4 = (uint64_t)createUserPML4Table();	//virtual address
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

/*struct PCB* get_next_proc() {

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
}*/



// void add_proc_to_list(struct PCB* proc)
// {
//     struct PCB* ptr = task_l;

//     if (proc->state == IDLE) {
//         return;
//     } else if (proc->state == RUNNING) {
//         proc->state = READY;
//     }

//     if (ptr == NULL) {
//         task_l = proc;
//     } else {
//         while (ptr->next != NULL) {
//             ptr = ptr->next;
//         }
        
//         ptr->next = proc;
//     }
// }

int check_proc_present()
{
    if(task_l != NULL)
        return 1;
    return 0;
}

struct PCB* get_next_proc() {
	
    if(task_l == NULL)
        return NULL;
    else
    {
        struct PCB* proc = task_l;
        task_l = task_l->next;
        return proc;
    }
    /*struct PCB* proc = task_l;
    while(proc != NULL)
    {
        if(proc->state == READY)
            return proc;
        proc = proc->next;
    }
    
	return NULL;*/
}

void add_proc_to_list(struct PCB* proc)
{
    if(proc->state == IDLE)
        return;
    else if(proc->state == RUNNING || proc->state == READY)
    {
        proc->state = READY;
    }
    else
        return;

    if(task_l == NULL)
    {
        task_l = proc;
    }
    else
    {
        struct PCB* temp = task_l;
        while(temp->next != NULL)
        {
            temp = temp->next;
        }
        temp->next = proc;
        proc->next = NULL;
    }
}

void print_task_list()
{
        struct PCB *t = task_l;
        while(t != NULL)
        {

                kprintf("pid: %d     ppid: %d\n",t->pid,t->ppid);

/*              kprintf2("task id %d", t->pid);
                kprintf2("task name %s", t->name);
                kprintf2("task state %d \n", t->state); */
                t = t->next;
        }

        kprintf("%s\n" ,"here");
        //kprintf("%s\n" ,"here");
}

void insert_stack_vma(struct mm_struct *mm, uint64_t start, uint64_t end, uint64_t size, uint64_t access_flags, uint64_t type) {

	struct vm_area_struct *new_vma = create_vm_area_struct(start, end, size, access_flags, type);
	struct vm_area_struct *temp = mm->vma_stack_list;
	int added = 0;
	if(mm->vma_stack_list == NULL) {
		mm->vma_stack_list = new_vma;
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

/*void get_next_task()
{
    struct PCB* t = task_l;
    if(t == NULL)
    {
        kprintf("No process to run\n");
    }
    else
    {
        while(t->next != NULL)
            t = t->next;
        t->next = current_proc;
        current_proc->next = NULL;
        current_proc = task_l;
        task_l = task_l->next;
        current_proc->next = NULL;
    }
}*/
/*

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

void insert_stack_vma(struct mm_struct *mm, uint64_t start, uint64_t end, uint64_t size, uint64_t access_flags, uint64_t type) {

	struct vm_area_struct *new_vma = create_vm_area_struct(start, end, size, access_flags, type);
	struct vm_area_struct *temp = mm->vma_stack_list;
	int added = 0;
	if(mm->vma_stack_list == NULL) {
		mm->vma_stack_list = new_vma;
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
}*/
