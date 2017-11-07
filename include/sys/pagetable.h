#ifndef _PAGE_TABLE_H_
#define _PAGE_TABLE_H_

#include "sys/defs.h"

#define PAGEINDEX 512
#define PAGESIZE 4096
#define PAGE_COUNT 24000

struct PAGE {
        struct PAGE *next;
	uint64_t physadd;
	//uint8_t use_cnt;
};

void loadKernel(uint64_t physbase, uint64_t physfree);

void initializePages(uint64_t physfree);

uint64_t getpage();

void free_page(struct PAGE *page);

void walkPageTables(uint64_t virtual_add, uint64_t phys_add);

void createPML4Table();

void updateCR3_Val();

void checkPDTPTable(uint64_t virtual_add, uint64_t phys_add);

void createPDTPTable(uint64_t virtual_add);

void checkPDTable(uint64_t virtual_add, uint64_t phys_add);

void createPDTable(uint64_t virtual_add);

void checkPTTable(uint64_t virtual_add, uint64_t phys_add);

void createPTTable(uint64_t virtual_add, uint64_t phys_add);

void checkEntry(uint64_t virtual_add, uint64_t phys_add);

#endif
