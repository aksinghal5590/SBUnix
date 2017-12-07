#ifndef _PCB_H_
#define _PCB_H_

#include <stdarg.h>
#include "sys/defs.h"
#define KSTACK_SIZE 100


struct PCB {
   uint64_t kstack[KSTACK_SIZE];
   uint64_t pid;
   uint64_t ppid;
   uint64_t rsp;
   uint64_t rip;
   char p_name[25]; 
   struct PCB *parent;
   uint64_t child_cnt;
   enum { RUNNING, SLEEPING, ZOMBIE, READY, IDLE, EXIT, WAIT } state;
   int exit_status;
   uint64_t pml4;
   struct mm_struct *mm;
   struct PCB *next;
   uint64_t wait_on_child_pid;
   uint64_t entry;
   uint8_t isUser;
   uint64_t stop;
};

struct mm_struct {
  struct vm_area_struct *vma_list;
  struct vm_area_struct *vma_stack_list;
};

struct vm_area_struct {
  uint64_t start;
  uint64_t end;
  uint64_t size;
  uint64_t access_flags;
  uint64_t type;
  struct vm_area_struct *next, *prev;
};

struct mm_struct* create_mm_struct();

struct vm_area_struct* create_vm_area_struct(uint64_t start, uint64_t end, uint64_t size, uint64_t access_flags, uint64_t type);

void insert_vma(struct mm_struct *mm, uint64_t start, uint64_t end, uint64_t size, uint64_t access_flags, uint64_t type);

void add_proc_to_list(struct PCB* proc);


struct PCB* get_free_task_struct();

struct PCB* get_next_proc();
void print_task_list();
void insert_stack_vma(struct mm_struct *mm, uint64_t start, uint64_t end, uint64_t size, uint64_t access_flags, uint64_t type);

void add_proc_to_list(struct PCB* proc);

struct PCB *create_new_proc(char *p_name, uint8_t isUser);

struct PCB* get_free_task_struct();

struct PCB* get_next_proc();

void print_task_list();

int check_proc_present();
//void get_next_task();

#endif
