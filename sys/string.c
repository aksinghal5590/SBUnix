#include "sys/string.h"
#include "sys/kprintf.h"

void* memcpy(void* dest, const void* src, size_t n) {
	char* d = (char*)dest;
	const char* s = (char*) src;
	while(n-- != 0) {
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

// return 0 if s1 starts with s2
int strstarts(char *s1, char *s2) {
	while((*s2) != '\0'  && (*s1) != '\0') {
		if((*s1) != (*s2)) {
			return -1;
		}
		s1++;
		s2++;
	}
	if((*s2) != '\0') {
		return -1;
	}
	return 0;
}
