#include "sys/freelist.h"
#include "sys/kprintf.h"

struct PAGE page_list[PAGE_COUNT];
struct PAGE* free_list=NULL;
int pageCount = 0;

int initializePages(uint64_t start, uint64_t end) {

	uint64_t baseAddress = (start/PAGESIZE + 1)*PAGESIZE;
	uint64_t endAddress = (end/PAGESIZE)*PAGESIZE;
	uint64_t count = (endAddress-baseAddress)/PAGESIZE;
        struct PAGE* tail = NULL;
        for(int i = 0; pageCount < PAGE_COUNT && i < count; pageCount++, i++) {
                page_list[pageCount].next = NULL;
                page_list[pageCount].physadd = baseAddress + (i * PAGESIZE);
                //page_list[i].use_cnt = 0;
                if(tail == NULL)
                        tail = page_list;
                else
                {
                        tail->next = (page_list + pageCount);
                        tail = (page_list + pageCount);
                }
        }
        if(free_list == NULL)
                free_list = page_list;
	return pageCount;
}

/*void initializePages(uint64_t physfree) {

	uint64_t baseAddress = (physfree/PAGESIZE + 1)*PAGESIZE;
	struct PAGE* tail = NULL;
        for(int i = 0; i < PAGE_COUNT; i++) {
		page_list[i].next = NULL;
                page_list[i].physadd = baseAddress + (i * PAGESIZE);
		//page_list[i].use_cnt = 0;
		if(tail == NULL)
			tail = page_list;
		else
		{
			tail->next = (page_list + i);
			tail = (page_list + i);
		}
        }
	if(free_list == NULL)
		free_list = page_list;
}*/

uint64_t getPage() {
	uint64_t fp;
	if (!(free_list)) {
		return 0;
	}

	//free_list->use_cnt = 1;
	fp = free_list->physadd;
	free_list = free_list->next;
	return fp;
}

void freePage(struct PAGE* p) {

	//p->use_cnt = 0;
        p->next = free_list;
        free_list = p;
}
