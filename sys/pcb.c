#include "sys/pcb.h"
#include "sys/kprintf.h"

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
