#include <sys/gdt.h>
#include <sys/kprintf.h>
#include <sys/tarfs.h>
#include <sys/ahci.h>
#include <sys/interrupt.h>
#include <sys/ahci.h>
#include "sys/pcb.h"
#include "sys/thread.h"
#include "sys/freelist.h"
#include "sys/vfs.h"
#include "sys/process_manager.h"
#include "sys/syscall.h"
#include "sys/elf64.h"

#define INITIAL_STACK_SIZE 4096

uint8_t initial_stack[INITIAL_STACK_SIZE]__attribute__((aligned(16)));

uint32_t* loader_stack;
struct PCB *threadA, *threadB;
extern char kernmem, physbase;
extern struct PCB* current_proc;
extern void initialSwitch(uint64_t rsp);
extern struct PCB* idle;
extern struct PCB* createThread();

int pgCount = 0;

void start(uint32_t *modulep, void *physbase, void *physfree)
{
  struct smap_t {
    uint64_t base, length;
    uint32_t type;
  }__attribute__((packed)) *smap;
  while(modulep[0] != 0x9001) modulep += modulep[1]+2;
  for(smap = (struct smap_t*)(modulep+2); smap < (struct smap_t*)((char*)modulep+modulep[1]+2*4); ++smap) {
    if (smap->type == 1 /* memory */ && smap->length != 0) {
      // kprintf("Available Physical Memory [%p-%p]\n", smap->base, smap->base + smap->length);

      if((uint64_t)physbase > smap->base + smap->length) {
	continue;
      } else if((uint64_t)physfree < smap->base) {
	pgCount = initializePages((uint64_t)smap->base, smap->base + smap->length);
      } else if(((uint64_t)physbase) >= smap->base && ((uint64_t)physfree) <= smap->base + smap->length) {
	pgCount = initializePages((uint64_t)physfree, smap->base + smap->length);
      }
    }
  }
  
  // kprintf("physbase %p\n", (uint64_t)physbase);
  // kprintf("physfree %p\n", (uint64_t)physfree);
  // kprintf("Kernmem %p\n", (uint64_t)&kernmem);
  // kprintf("tarfs in [%p:%p]\n", &_binary_tarfs_start, &_binary_tarfs_end);
  // kprintf("Page Count: %d\n", pgCount);
  loadKernel((uint64_t)physbase, (uint64_t)physfree);
  init_tarfs();
  init_vfs();
  initSyscalls();
  initInterrupts();
  // kprintf("Loaded our own kernel!!!!Its working!!!!!\n");

  __asm__ __volatile__("movq %0, %%rbp" : :"a"(&initial_stack[0]));
  __asm__ __volatile__("movq %0, %%rsp" : :"a"(&initial_stack[INITIAL_STACK_SIZE]));

  initIdleProcess();

  clearScreen();

  //read_file("/bin/init", NULL, NULL);
  print_task_list();
  //performContextSwitch(eEntry);
  // if(eEntry);
  struct PCB* t = idle;
  if(t != NULL)
    initialSwitch(t->rsp);

  while(1);
}

void boot(void)
{
  // note: function changes rsp, local stack variables can't be practically used
  register char *temp1, *temp2;

  for(temp2 = (char*)0xb8001; temp2 < (char*)0xb8000+160*25; temp2 += 2) *temp2 = 7 /* white */;
  __asm__ volatile (
    "cli;"
    "movq %%rsp, %0;"
    "movq %1, %%rsp;"
    :"=g"(loader_stack)
    :"r"(&initial_stack[INITIAL_STACK_SIZE])
  );
  init_gdt();
  start(
    (uint32_t*)((char*)(uint64_t)loader_stack[3] + (uint64_t)&kernmem - (uint64_t)&physbase),
    (uint64_t*)&physbase,
    (uint64_t*)(uint64_t)loader_stack[4]
  );
  for(
    temp1 = "!!!!! start() returned !!!!!", temp2 = (char*)0xb8000;
    *temp1;
    temp1 += 1, temp2 += 2
  ) *temp2 = *temp1;
  while(1) __asm__ volatile ("hlt");
}
