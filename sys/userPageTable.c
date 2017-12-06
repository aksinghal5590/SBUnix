#include "sys/userPageTable.h"
#include "sys/freelist.h"
#include "sys/kprintf.h"
#include "sys/string.h"
#include "sys/pagetable.h"

extern char kernmem;

extern uint64_t* globalPdtpTable;

uint64_t* createUserProcess()
{
	return (createUserPML4Table());
}

uint64_t* createUserPML4Table() {

	uint64_t* userPml4Table = (uint64_t*)getPage();
	uint64_t* v_userPml4Table = (uint64_t*)(VIRTUAL_BASE + (uint64_t)userPml4Table);
	for(int i = 0; i < PAGEINDEX; i++) {
		*(v_userPml4Table + i) = 0x0;
	}
	
	//uint64_t v_address = (uint64_t)&kernmem;
	*(v_userPml4Table + (((VIRTUAL_BASE)>>39) & 0x1FF)) = ALL_ZERO | (((uint64_t)globalPdtpTable & GET_40_BITS)) | 0x003;
	return userPml4Table;
}

void updateUserCR3_Val(uint64_t userPml4Table) {

	uint64_t ptr = userPml4Table & GET_40_BITS;
	
	__asm__(
		"movq %0, %%cr3 \n\t"
		:
		:"r"(ptr)
		:"cc"
	);
}

void walkUserPageTables(uint64_t userPml4Table, uint64_t vmaAddress) {

	checkUserPDTPTable(userPml4Table, vmaAddress);
}

void checkUserPDTPTable(uint64_t userPml4Table, uint64_t vmaAddress)
{
	uint64_t* v_userPml4Table = (uint64_t*)(VIRTUAL_BASE + userPml4Table);
	uint64_t pml4_val = *(v_userPml4Table + ((vmaAddress>>39) & 0x1FF));
	if(!(pml4_val & 0x1))
	{
		createUserPDTPTable(v_userPml4Table, vmaAddress);
	}
	checkUserPDTable(v_userPml4Table, vmaAddress);
}

void createUserPDTPTable(uint64_t* v_userPml4Table, uint64_t vmaAddress) {

	uint64_t* userPdtpTable = (uint64_t*)getPage();
	uint64_t* v_userPdtpTable = (uint64_t*)(VIRTUAL_BASE + (uint64_t)userPdtpTable);
	*(v_userPml4Table + ((vmaAddress>>39) & 0x1FF)) = ALL_ZERO | (((uint64_t)userPdtpTable & GET_40_BITS)) | 0x007;
    for(int i = 0; i < PAGEINDEX; i++) {
		*(v_userPdtpTable+i) = 0x0;
	}
}

void checkUserPDTable(uint64_t* v_userPml4Table, uint64_t vmaAddress)
{
	uint64_t pml4_val = *(v_userPml4Table + ((vmaAddress>>39) & 0x1FF));
	uint64_t pdtp_base = pml4_val & GET_40_BITS;
	uint64_t* pdtp_val_ptr = (uint64_t*)(VIRTUAL_BASE + pdtp_base + (((vmaAddress>>30) & 0x1FF) * sizeof(uint64_t)));
	uint64_t pdtp_val = (uint64_t)(*pdtp_val_ptr);	
	if(!(pdtp_val & 0x1))
	{
		createUserPDTable(v_userPml4Table, vmaAddress);
	}
	checkUserPTTable(v_userPml4Table, vmaAddress);
}

void createUserPDTable(uint64_t* v_userPml4Table, uint64_t vmaAddress) {

	uint64_t* userPdTable = (uint64_t*)getPage();
	uint64_t* v_userPdTable = (uint64_t*)(VIRTUAL_BASE + (uint64_t)userPdTable);
	uint64_t pml4_val = *(v_userPml4Table + ((vmaAddress>>39) & 0x1FF));
	uint64_t pdtp_base = pml4_val & GET_40_BITS;
	uint64_t* v_userPdtpTable = (uint64_t*)(VIRTUAL_BASE + pdtp_base + (((vmaAddress>>30) & 0x1FF) * sizeof(uint64_t)));
	*(v_userPdtpTable) = ALL_ZERO | (((uint64_t)userPdTable & GET_40_BITS)) | 0x007;
	for(int i = 0; i < PAGEINDEX; i++) {
		*(v_userPdTable+i) = 0x0;
	}
}

void checkUserPTTable(uint64_t* v_userPml4Table, uint64_t vmaAddress)
{
	uint64_t pml4_val = *(v_userPml4Table + ((vmaAddress>>39) & 0x1FF));
	uint64_t pdtp_base = pml4_val & GET_40_BITS;
	uint64_t* pdtp_val_ptr = (uint64_t*)(VIRTUAL_BASE + pdtp_base + (((vmaAddress>>30) & 0x1FF) * sizeof(uint64_t)));
	uint64_t pdtp_val = (uint64_t)(*pdtp_val_ptr);	
	uint64_t pd_base = pdtp_val & GET_40_BITS;
	uint64_t* pd_val_ptr = (uint64_t*)(VIRTUAL_BASE + pd_base + (((vmaAddress>>21) & 0x1FF) * sizeof(uint64_t)));
	uint64_t pd_val = (uint64_t)(*pd_val_ptr);
	if(!(pd_val & 0x1))
	{
		createUserPTTable(v_userPml4Table, vmaAddress);
	}
	else
	{
		checkUserEntry(v_userPml4Table, vmaAddress);
	}
}

void createUserPTTable(uint64_t* v_userPml4Table, uint64_t vmaAddress) {

	uint64_t* userPtTable = (uint64_t*)getPage();
	uint64_t* v_userPtTable = (uint64_t*)(VIRTUAL_BASE + (uint64_t)userPtTable);

	uint64_t pml4_val = *(v_userPml4Table + ((vmaAddress>>39) & 0x1FF));
	uint64_t pdtp_base = pml4_val & GET_40_BITS;
	uint64_t* pdtp_val_ptr = (uint64_t*)(VIRTUAL_BASE + pdtp_base + (((vmaAddress>>30) & 0x1FF) * sizeof(uint64_t)));
	uint64_t pdtp_val = (uint64_t)(*pdtp_val_ptr);	
	uint64_t pd_base = pdtp_val & GET_40_BITS;
	uint64_t* v_userPdTable = (uint64_t*)(VIRTUAL_BASE + pd_base + (((vmaAddress>>21) & 0x1FF) * sizeof(uint64_t)));
	
	*(v_userPdTable) = ALL_ZERO | (((uint64_t)userPtTable & GET_40_BITS)) | 0x007;
	for(int i = 0; i < PAGEINDEX; i++) {
		*(v_userPtTable+i) = 0x0;
	}
	uint64_t phys_add = getPage();
	*(v_userPtTable + ((vmaAddress>>12) & 0x1FF)) = ALL_ZERO | ((phys_add & GET_40_BITS)) | 0x007;
}

void checkUserEntry(uint64_t* v_userPml4Table, uint64_t vmaAddress)
{
	uint64_t pml4_val = *(v_userPml4Table + ((vmaAddress>>39) & 0x1FF));
	uint64_t pdtp_base = pml4_val & GET_40_BITS;
	uint64_t* pdtp_val_ptr = (uint64_t*)(VIRTUAL_BASE + pdtp_base + (((vmaAddress>>30) & 0x1FF)* sizeof(uint64_t)));
	uint64_t pdtp_val = (uint64_t)(*pdtp_val_ptr);	
	uint64_t pd_base = pdtp_val & GET_40_BITS;
	uint64_t* pd_val_ptr = (uint64_t*)(VIRTUAL_BASE + pd_base + (((vmaAddress>>21) & 0x1FF) * sizeof(uint64_t)));
	uint64_t pd_val = (uint64_t)(*pd_val_ptr);
	uint64_t pt_base = pd_val & GET_40_BITS;
	uint64_t* pt_val_ptr = (uint64_t*)(VIRTUAL_BASE + pt_base + (((vmaAddress>>12) & 0x1FF) * sizeof(uint64_t)));
	uint64_t pt_val = (uint64_t)(*pt_val_ptr);

	if(!(pt_val & 0x01))
	{
		uint64_t phys_add = getPage();
		*(pt_val_ptr) = ALL_ZERO | ((phys_add & GET_40_BITS)) | 0x007;
	}
}

void copyUserData(uint64_t pml4_add, uint64_t vmaAddress, uint64_t* vAddress, uint64_t len)
{
	uint64_t* v_userPml4Table = (uint64_t*)(VIRTUAL_BASE + pml4_add);
	uint64_t pml4_val = *(v_userPml4Table + ((vmaAddress>>39) & 0x1FF));
	uint64_t pdtp_base = pml4_val & GET_40_BITS;
	uint64_t* pdtp_val_ptr = (uint64_t*)(VIRTUAL_BASE + pdtp_base + (((vmaAddress>>30) & 0x1FF)* sizeof(uint64_t)));
	uint64_t pdtp_val = (uint64_t)(*pdtp_val_ptr);	
	uint64_t pd_base = pdtp_val & GET_40_BITS;
	uint64_t* pd_val_ptr = (uint64_t*)(VIRTUAL_BASE + pd_base + (((vmaAddress>>21) & 0x1FF) * sizeof(uint64_t)));
	uint64_t pd_val = (uint64_t)(*pd_val_ptr);
	uint64_t pt_base = pd_val & GET_40_BITS;
	uint64_t* pt_val_ptr = (uint64_t*)(VIRTUAL_BASE + pt_base + (((vmaAddress>>12) & 0x1FF) * sizeof(uint64_t)));
	uint64_t pt_val = (uint64_t)(*pt_val_ptr);
	if(pt_val & 0x01)
	{
		uint64_t* v_pt_val = (uint64_t*) (VIRTUAL_BASE + (pt_val & GET_40_BITS));
		memcpy((void*)v_pt_val, (void*)vAddress, len);
		/*char* src = (char*)vmaAddress;
		char* dest = (char*)v_pt_val;
		
		for(int i = 0; i < 4096; i++)
		{
			dest = src;
			kprintf("Value of dest is: %d\n", dest);
			kprintf("Value of src is: %d\n", src);
			dest++;
			src++;
		}*/
	}
}

int user_page_exist(uint64_t pml4Address, uint64_t vAddress)
{
	uint64_t* v_userPml4Table = (uint64_t*)(VIRTUAL_BASE + pml4Address);
	uint64_t pml4_val = *(v_userPml4Table + ((vAddress>>39) & 0x1FF));
	uint64_t pdtp_base = pml4_val & GET_40_BITS;
	uint64_t* pdtp_val_ptr = (uint64_t*)(VIRTUAL_BASE + pdtp_base + (((vAddress>>30) & 0x1FF)* sizeof(uint64_t)));
	uint64_t pdtp_val = (uint64_t)(*pdtp_val_ptr);	
	uint64_t pd_base = pdtp_val & GET_40_BITS;
	uint64_t* pd_val_ptr = (uint64_t*)(VIRTUAL_BASE + pd_base + (((vAddress>>21) & 0x1FF) * sizeof(uint64_t)));
	uint64_t pd_val = (uint64_t)(*pd_val_ptr);
	uint64_t pt_base = pd_val & GET_40_BITS;
	uint64_t* pt_val_ptr = (uint64_t*)(VIRTUAL_BASE + pt_base + (((vAddress>>12) & 0x1FF) * sizeof(uint64_t)));
	uint64_t pt_val = (uint64_t)(*pt_val_ptr);
	if(pt_val & 0x01)
        return 1;
    return 0;
}

void useExistingPage(uint64_t pml4Address, uint64_t vAddress, uint64_t oldPhysAddress)
{
	uint64_t* v_userPml4Table = (uint64_t*)(VIRTUAL_BASE + pml4Address);
	uint64_t pml4_val = *(v_userPml4Table + ((vAddress>>39) & 0x1FF));
	uint64_t pdtp_base = pml4_val & GET_40_BITS;
	uint64_t* pdtp_val_ptr = (uint64_t*)(VIRTUAL_BASE + pdtp_base + (((vAddress>>30) & 0x1FF)* sizeof(uint64_t)));
	uint64_t pdtp_val = (uint64_t)(*pdtp_val_ptr);	
	uint64_t pd_base = pdtp_val & GET_40_BITS;
	uint64_t* pd_val_ptr = (uint64_t*)(VIRTUAL_BASE + pd_base + (((vAddress>>21) & 0x1FF) * sizeof(uint64_t)));
	uint64_t pd_val = (uint64_t)(*pd_val_ptr);
	uint64_t pt_base = pd_val & GET_40_BITS;
	uint64_t* pt_val_ptr = (uint64_t*)(VIRTUAL_BASE + pt_base + (((vAddress>>12) & 0x1FF) * sizeof(uint64_t)));
	*(pt_val_ptr) = ALL_ZERO | ((oldPhysAddress & GET_40_BITS)) | 0x007;
}

uint64_t* getPTTableEntry(uint64_t pml4Address, uint64_t vAddress)
{
	uint64_t* v_userPml4Table = (uint64_t*)(VIRTUAL_BASE + pml4Address);
	uint64_t pml4_val = *(v_userPml4Table + ((vAddress>>39) & 0x1FF));
	uint64_t pdtp_base = pml4_val & GET_40_BITS;
	uint64_t* pdtp_val_ptr = (uint64_t*)(VIRTUAL_BASE + pdtp_base + (((vAddress>>30) & 0x1FF)* sizeof(uint64_t)));
	uint64_t pdtp_val = (uint64_t)(*pdtp_val_ptr);	
	uint64_t pd_base = pdtp_val & GET_40_BITS;
	uint64_t* pd_val_ptr = (uint64_t*)(VIRTUAL_BASE + pd_base + (((vAddress>>21) & 0x1FF) * sizeof(uint64_t)));
	uint64_t pd_val = (uint64_t)(*pd_val_ptr);
	uint64_t pt_base = pd_val & GET_40_BITS;
	uint64_t* pt_val_ptr = (uint64_t*)(VIRTUAL_BASE + pt_base + (((vAddress>>12) & 0x1FF) * sizeof(uint64_t)));
    return pt_val_ptr;
}
