#include "sys/string.h"
#include "sys/kprintf.h"

void* memcpy(void* dest, const void* src, size_t n)
{
	char* d = (char*)dest;
	const char* s = (char*) src;
	while(n-- != 0)
	{
		/*if(*s != 0)
			kprintf("value of s is:%d\n", *s);*/
		*d = *s;
		/*if(*d != 0)
			kprintf("value of d is:%d\n", *d);*/
		d++;
		s++;
	}
	return dest;
}
