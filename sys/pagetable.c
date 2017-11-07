#include "sys/pagetable.h"
#include "sys/kprintf.h"

extern char kernmem;

uint64_t* pml4Table = NULL;

uint64_t* pdtpTable = NULL;

uint64_t* pdTable = NULL;

uint64_t* ptTable = NULL;

uint64_t cr3_value;

struct PAGE page_list[PAGE_COUNT];
struct PAGE* free_list = NULL;

void loadKernel(uint64_t physbase, uint64_t physfree) {

	initializePages(physfree);
        for(uint64_t i = physbase; i < physfree; i += 0x1000)
	{
		uint64_t v_address = ((uint64_t)&kernmem - physbase + i);
        	walkPageTables(v_address, i);
	}
        walkPageTables(VIDEO_MEM_VIRTUAL, VIDEO_MEM);
	updateCR3_Val();
}

void initializePages(uint64_t physfree) {

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
}

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

void walkPageTables(uint64_t virtual_add, uint64_t phys_add) {

	if(pml4Table == NULL)
	{
		createPML4Table();
	}
		
	checkPDTPTable(virtual_add, phys_add);
}

void createPML4Table() {
	pml4Table = (uint64_t*)getPage();
	for(int i = 0; i < PAGEINDEX; i++) {
		*(VIRTUAL_BASE + pml4Table + i) = 0x02;
	}
}

void updateCR3_Val() {
	uint64_t ptr = ((uint64_t)pml4Table) & GET_40_BITS;
	//cr3_value = ALL_ZERO | (ptr << 12) | 0x000;
	cr3_value = ptr;
	__asm__(
		"movq %0, %%cr3 \n\t"
		:
		:"r"(cr3_value)
		:"cc"
	);
}

void checkPDTPTable(uint64_t virtual_add, uint64_t phys_add)
{
	uint64_t pml4_val = *(VIRTUAL_BASE + pml4Table + ((virtual_add>>39) & 0x1FF));
	if(!(pml4_val & 0x1))
	{
		createPDTPTable(virtual_add);
	}
	checkPDTable(virtual_add, phys_add);
	
}

void createPDTPTable(uint64_t virtual_add) {
	pdtpTable = (uint64_t*)getPage();
	*(VIRTUAL_BASE + pml4Table + ((virtual_add>>39) & 0x1FF)) = ALL_ZERO | (((uint64_t)pdtpTable & GET_40_BITS)) | 0x003;
	for(int i = 0; i < PAGEINDEX; i++) {
		*(VIRTUAL_BASE + pdtpTable+i) = 0x02;
	}
}

void checkPDTable(uint64_t virtual_add, uint64_t phys_add)
{
	uint64_t pml4_val = *(VIRTUAL_BASE + pml4Table + ((virtual_add>>39) & 0x1FF));
	uint64_t* pdtp_base = (uint64_t*)(pml4_val & GET_40_BITS);
	uint64_t pdtp_val = *(VIRTUAL_BASE + pdtp_base + ((virtual_add>>30) & 0x1FF));	
	if(!(pdtp_val & 0x1))
	{
		createPDTable(virtual_add);
	}
	checkPTTable(virtual_add, phys_add);
}

void createPDTable(uint64_t virtual_add) {
	pdTable = (uint64_t*)getPage();
	*(VIRTUAL_BASE + pdtpTable + ((virtual_add>>30) & 0x1FF)) = ALL_ZERO | (((uint64_t)pdTable & GET_40_BITS)) | 0x003;
	for(int i = 0; i < PAGEINDEX; i++) {
		*(VIRTUAL_BASE + pdTable+i) = 0x02;
	}
}

void checkPTTable(uint64_t virtual_add, uint64_t phys_add)
{
	uint64_t pml4_val = *(VIRTUAL_BASE + pml4Table + ((virtual_add>>39) & 0x1FF));
	uint64_t* pdtp_base = (uint64_t*)(pml4_val & GET_40_BITS);
	uint64_t pdtp_val = *(VIRTUAL_BASE + pdtp_base + ((virtual_add>>30) & 0x1FF));
	uint64_t* pd_base = (uint64_t*)(pdtp_val & GET_40_BITS);
	uint64_t pd_val = *(VIRTUAL_BASE + pd_base + ((virtual_add>>21) & 0x1FF));
	if(!(pd_val & 0x1))
	{
		createPTTable(virtual_add, phys_add);
	}
	else
	{
		checkEntry(virtual_add, phys_add);
	}
}

void createPTTable(uint64_t virtual_add, uint64_t phys_add) {
	ptTable = (uint64_t*)getPage();
	*(VIRTUAL_BASE + pdTable + ((virtual_add>>21) & 0x1FF)) = ALL_ZERO | (((uint64_t)ptTable & GET_40_BITS)) | 0x003;
	for(int i = 0; i < PAGEINDEX; i++) {
		*(ptTable+i) = 0x02;
	}
	*(VIRTUAL_BASE + ptTable + ((virtual_add>>12) & 0x1FF)) = ALL_ZERO | ((phys_add & GET_40_BITS)) | 0x003;
}

void checkEntry(uint64_t virtual_add, uint64_t phys_add)
{
	uint64_t pml4_val = *(VIRTUAL_BASE + pml4Table + ((virtual_add>>39) & 0x1FF));
	uint64_t* pdtp_base = (uint64_t*)(pml4_val & GET_40_BITS);
	uint64_t pdtp_val = *(VIRTUAL_BASE + pdtp_base + ((virtual_add>>30) & 0x1FF));
	uint64_t* pd_base = (uint64_t*)(pdtp_val & GET_40_BITS);
	uint64_t pd_val = *(VIRTUAL_BASE + pd_base + ((virtual_add>>21) & 0x1FF));
	uint64_t* pt_base = (uint64_t*)(pd_val & GET_40_BITS);
	uint64_t pt_val = *(VIRTUAL_BASE + pt_base + ((virtual_add>>12) & 0x1FF));
	if(!(pt_val & 0x01))
	{
		*(VIRTUAL_BASE + pt_base + ((virtual_add>>12) & 0x1FF)) = ALL_ZERO | ((phys_add & GET_40_BITS)) | 0x003;
	}
}
