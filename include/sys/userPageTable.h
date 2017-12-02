#ifndef _USER_PAGE_TABLE_H_
#define _USER_PAGE_TABLE_H_

#include "sys/defs.h"


uint64_t* createUserProcess();

uint64_t* createUserPML4Table();

void updateUserCR3_Val(uint64_t userPml4Table);

void walkUserPageTables(uint64_t userPml4Table, uint64_t vmaAddress);

void checkUserPDTPTable(uint64_t userPml4Table, uint64_t vmaAddress);

void createUserPDTPTable(uint64_t* userPml4Table, uint64_t vmaAddress);

void checkUserPDTable(uint64_t* userPml4Table, uint64_t vmaAddress);

void createUserPDTable(uint64_t* userPml4Table, uint64_t vmaAddress);

void checkUserPTTable(uint64_t* userPml4Table, uint64_t vmaAddress);

void createUserPTTable(uint64_t* userPml4Table, uint64_t vmaAddress);

void checkUserEntry(uint64_t* userPml4Table, uint64_t vmaAddress);

void copyUserData(uint64_t pml4_add, uint64_t vmaAddress, uint64_t* vAddress, uint64_t len);

#endif
