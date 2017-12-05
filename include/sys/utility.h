#ifndef _UTILITY_H_
#define _UTILITY_H_

#include "sys/defs.h"

void loadCR3(uint64_t pml4);
uint64_t getCR2Val();
uint64_t getCR3Val();

#endif
