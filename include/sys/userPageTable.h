#ifndef _USER_PAGE_TABLE_H_
#define _USER_PAGE_TABLE_H_

#include "sys/defs.h"


uint64_t* createUserProcess();

uint64_t* createUserPML4Table();

void updateUserCR3_Val(uint64_t userPml4Table);

void walkUserPageTables(uint64_t userPml4Table, uint64_t vmaAddress, uint64_t oldPhysAddress);

void checkUserPDTPTable(uint64_t userPml4Table, uint64_t vmaAddress, uint64_t oldPhysAddress);

void createUserPDTPTable(uint64_t* userPml4Table, uint64_t vmaAddress, uint64_t oldPhysAddress);

void checkUserPDTable(uint64_t* userPml4Table, uint64_t vmaAddress, uint64_t oldPhysAddress);

void createUserPDTable(uint64_t* userPml4Table, uint64_t vmaAddress, uint64_t oldPhysAddress);

void checkUserPTTable(uint64_t* userPml4Table, uint64_t vmaAddress, uint64_t oldPhysAddress);

void createUserPTTable(uint64_t* userPml4Table, uint64_t vmaAddress, uint64_t oldPhysAddress);

void checkUserEntry(uint64_t* userPml4Table, uint64_t vmaAddress, uint64_t oldPhysAddress);

void copyUserData(uint64_t pml4_add, uint64_t vmaAddress, uint64_t* vAddress, uint64_t len);

int user_page_exist(uint64_t pml4Address, uint64_t vAddress);

void useExistingPage(uint64_t pml4Address, uint64_t vAddress, uint64_t oldPhysAddress);

uint64_t* getPTTableEntry(uint64_t pml4Address, uint64_t vAddress);

#endif
