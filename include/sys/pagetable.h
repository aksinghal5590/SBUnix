#ifndef _PAGE_TABLE_H_
#define _PAGE_TABLE_H_

#include "sys/defs.h"

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
