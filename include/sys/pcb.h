#ifndef _PCB_H_
#define _PCB_H_

#include <stdarg.h>
#include "sys/defs.h"

struct PCB {
   uint64_t kstack[100];
   uint64_t pid;
   uint64_t rsp;
   enum { RUNNING, SLEEPING, ZOMBIE } state;
   int exit_status;
   struct mm_struct *mm;
};

struct mm_struct {
  struct vm_area_struct *vma_list;
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

#endif
