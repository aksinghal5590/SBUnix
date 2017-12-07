#ifndef _PAGE_FAULT_H_
#define _PAFE_FAULT_H_

#include "sys/defs.h"

void divideByZeroHandler();
void tssFaultHandler();
void gpfFaultHandler();
void pageFaultHandler();

#endif
