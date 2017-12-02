#ifndef _FREE_LIST_H_
#define _FREE_LIST_H_

#include "sys/defs.h"

struct PAGE {
        struct PAGE *next;
	uint64_t physadd;
	//uint8_t use_cnt;
};

//void initializePages(uint64_t physfree);

int initializePages(uint64_t start, uint64_t end);

uint64_t getPage();

void freePage(struct PAGE* page);

#endif
