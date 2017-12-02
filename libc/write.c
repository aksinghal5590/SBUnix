#include <stdio.h>
#include "sys/defs.h"
#include "sys/syscall.h"

extern void userWriteTarfs(uint64_t fd, uint64_t data, uint64_t len, uint64_t sysNum);

void write()
{
	char* data = "SNOWOS";	
	userWriteTarfs(1, (uint64_t)data, 6, 1);
	data = "VAIBHAV";
	userWriteTarfs(1, (uint64_t)data, 7, 1);
	data = "AKSHAT";
	userWriteTarfs(1, (uint64_t)data, 6, 1);
	data = "ASHISH";
	userWriteTarfs(1, (uint64_t)data, 6, 1);
}

