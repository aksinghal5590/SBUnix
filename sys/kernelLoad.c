#include "sys/kernelLoad.h"
#include "sys/pagetable.h"
#include "sys/freelist.h"
#include "sys/kprintf.h"

extern char kernmem;

void loadKernel(uint64_t physbase, uint64_t physfree) {

//	initializePages(physfree);
        for(uint64_t i = physbase; i < physfree+(1000*(0x1000)); i += 0x1000)
	{
		uint64_t v_address = ((uint64_t)&kernmem - physbase + i);
        	walkPageTables(v_address, i);
	}
	updateCR3_Val();
        walkPageTables(VIDEO_MEM_VIRTUAL, VIDEO_MEM);
}

uint64_t* kmalloc(int64_t size)
{
	uint64_t* baseAddress = NULL;
	uint8_t isFirst = 1;
	kprintf("size is: %d\n", size);
	while(size > 0)
	{
		uint64_t pg = getPage();
		uint64_t v_address = VIRTUAL_BASE + pg;
		walkPageTables(v_address, pg);
		kprintf("New page at: %x \n", pg);
		size = size - 4096;
		kprintf("size is: %d\n", size);
		if(isFirst)
		{
			isFirst = 0;
			baseAddress =(uint64_t*) v_address;
		}
	}
	return baseAddress;
}
