#ifndef _KERNEL_LOAD_H_
#define _KERNEL_LOAD_H_

#include "sys/defs.h"

void loadKernel(uint64_t physbase, uint64_t physfree);

uint64_t* kmalloc(int64_t size);

#endif
