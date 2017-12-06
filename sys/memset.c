#include "sys/kprintf.h"

void* memset(void* ptr, int value, size_t num)
{
	uint8_t* ptr1 = ptr;
	while(num--)
	{
		*ptr1++ = value;
	}
	return ptr;
}
