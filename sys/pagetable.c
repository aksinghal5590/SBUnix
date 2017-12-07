#include "sys/defs.h"
#include "sys/freelist.h"
#include "sys/pagetable.h"
#include "sys/kprintf.h"

extern char kernmem;

uint64_t* globalPdtpTable = NULL;

uint64_t* pml4Table = NULL;

uint64_t* v_pml4Table = NULL;

uint64_t* pdtpTable = NULL;

uint64_t* v_pdtpTable = NULL;

uint64_t* pdTable = NULL;

uint64_t* v_pdTable = NULL;

uint64_t* ptTable = NULL;

uint64_t* v_ptTable = NULL;

uint64_t cr3_value;

void walkPageTables(uint64_t virtual_add, uint64_t phys_add) {

	if(pml4Table == NULL)
	{
		createPML4Table();
	}	
	checkPDTPTable(virtual_add, phys_add);
}

void createPML4Table() {

	pml4Table = (uint64_t*)getPage();
    struct PAGE* p = getPageStruct((uint64_t)pml4Table);
    p->use_cnt = 1;
	v_pml4Table = (uint64_t*)(VIRTUAL_BASE + (uint64_t)pml4Table);
	for(int i = 0; i < PAGEINDEX; i++) {
		*(v_pml4Table + i) = 0x02;
	}
}

void updateCR3_Val() {

	uint64_t ptr = ((uint64_t)pml4Table) & GET_40_BITS;
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
	uint64_t pml4_val = *(v_pml4Table + ((virtual_add>>39) & 0x1FF));
	if(!(pml4_val & 0x1))
	{
		createPDTPTable(virtual_add);
	}
	checkPDTable(virtual_add, phys_add);
}

void createPDTPTable(uint64_t virtual_add) {

	pdtpTable = (uint64_t*)getPage();
    struct PAGE* p = getPageStruct((uint64_t)pdtpTable);
    p->use_cnt = 1;
    if(globalPdtpTable == NULL)
        globalPdtpTable = pdtpTable;
	v_pdtpTable = (uint64_t*)(VIRTUAL_BASE + (uint64_t)pdtpTable);
	*(v_pml4Table + ((virtual_add>>39) & 0x1FF)) = ALL_ZERO | (((uint64_t)pdtpTable & GET_40_BITS)) | 0x003;
	for(int i = 0; i < PAGEINDEX; i++) {
		*(v_pdtpTable+i) = 0x02;
	}
}

void checkPDTable(uint64_t virtual_add, uint64_t phys_add)
{
	uint64_t pml4_val = *(v_pml4Table + ((virtual_add>>39) & 0x1FF));
	uint64_t pdtp_base = pml4_val & GET_40_BITS;
	uint64_t* pdtp_val_ptr = (uint64_t*)(VIRTUAL_BASE + pdtp_base + (((virtual_add>>30) & 0x1FF) * sizeof(uint64_t)));
	uint64_t pdtp_val = (uint64_t)(*pdtp_val_ptr);	
	if(!(pdtp_val & 0x1))
	{
		createPDTable(virtual_add);
	}
	checkPTTable(virtual_add, phys_add);
}

void createPDTable(uint64_t virtual_add) {

	pdTable = (uint64_t*)getPage();
    struct PAGE* p = getPageStruct((uint64_t)pdTable);
    p->use_cnt = 1;
	v_pdTable = (uint64_t*)(VIRTUAL_BASE + (uint64_t)pdTable);
	*(v_pdtpTable + ((virtual_add>>30) & 0x1FF)) = ALL_ZERO | (((uint64_t)pdTable & GET_40_BITS)) | 0x003;
	for(int i = 0; i < PAGEINDEX; i++) {
		*(v_pdTable+i) = 0x02;
	}
}

void checkPTTable(uint64_t virtual_add, uint64_t phys_add)
{
	uint64_t pml4_val = *(v_pml4Table + ((virtual_add>>39) & 0x1FF));
	uint64_t pdtp_base = pml4_val & GET_40_BITS;
	uint64_t* pdtp_val_ptr = (uint64_t*)(VIRTUAL_BASE + pdtp_base + (((virtual_add>>30) & 0x1FF) * sizeof(uint64_t)));
	uint64_t pdtp_val = (uint64_t)(*pdtp_val_ptr);	
	uint64_t pd_base = pdtp_val & GET_40_BITS;
	uint64_t* pd_val_ptr = (uint64_t*)(VIRTUAL_BASE + pd_base + (((virtual_add>>21) & 0x1FF) * sizeof(uint64_t)));
	uint64_t pd_val = (uint64_t)(*pd_val_ptr);
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
    struct PAGE* p = getPageStruct((uint64_t)ptTable);
    p->use_cnt = 1;
	v_ptTable = (uint64_t*)(VIRTUAL_BASE + (uint64_t)ptTable);
	*(v_pdTable + ((virtual_add>>21) & 0x1FF)) = ALL_ZERO | (((uint64_t)ptTable & GET_40_BITS)) | 0x003;
	for(int i = 0; i < PAGEINDEX; i++) {
		*(v_ptTable+i) = 0x02;
	}
	*(v_ptTable + ((virtual_add>>12) & 0x1FF)) = ALL_ZERO | ((phys_add & GET_40_BITS)) | 0x003;
}

void checkEntry(uint64_t virtual_add, uint64_t phys_add)
{
	uint64_t pml4_val = *(v_pml4Table + ((virtual_add>>39) & 0x1FF));
	uint64_t pdtp_base = pml4_val & GET_40_BITS;
	uint64_t* pdtp_val_ptr = (uint64_t*)(VIRTUAL_BASE + pdtp_base + (((virtual_add>>30) & 0x1FF)* sizeof(uint64_t)));
	uint64_t pdtp_val = (uint64_t)(*pdtp_val_ptr);	
	uint64_t pd_base = pdtp_val & GET_40_BITS;
	uint64_t* pd_val_ptr = (uint64_t*)(VIRTUAL_BASE + pd_base + (((virtual_add>>21) & 0x1FF) * sizeof(uint64_t)));
	uint64_t pd_val = (uint64_t)(*pd_val_ptr);
	uint64_t pt_base = pd_val & GET_40_BITS;
	uint64_t* pt_val_ptr = (uint64_t*)(VIRTUAL_BASE + pt_base + (((virtual_add>>12) & 0x1FF) * sizeof(uint64_t)));
	uint64_t pt_val = (uint64_t)(*pt_val_ptr);

	if(!(pt_val & 0x01))
	{
		*(pt_val_ptr) = ALL_ZERO | ((phys_add & GET_40_BITS)) | 0x003;
	}
}
