#include "sys/userPageTable.h"
#include "sys/freelist.h"
#include "sys/kprintf.h"
#include "sys/string.h"
#include "sys/pagetable.h"
#include "sys/utility.h"

extern char kernmem;

extern uint64_t* globalPdtpTable;

uint64_t* createUserProcess()
{
	return (createUserPML4Table());
}

uint64_t* createUserPML4Table() {

	uint64_t* userPml4Table = (uint64_t*)getPage();
	struct PAGE* p = getPageStruct((uint64_t)userPml4Table);
    	p->use_cnt = 1;
	uint64_t* v_userPml4Table = (uint64_t*)(VIRTUAL_BASE + (uint64_t)userPml4Table);
	for(int i = 0; i < PAGEINDEX; i++) {
		*(v_userPml4Table + i) = 0x0;
	}
	
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

void walkUserPageTables(uint64_t userPml4Table, uint64_t vmaAddress, uint64_t oldPhysAddress) {

	checkUserPDTPTable(userPml4Table, vmaAddress, oldPhysAddress);
}

void checkUserPDTPTable(uint64_t userPml4Table, uint64_t vmaAddress, uint64_t oldPhysAddress)
{
	uint64_t* v_userPml4Table = (uint64_t*)(VIRTUAL_BASE + userPml4Table);
	uint64_t pml4_val = *(v_userPml4Table + ((vmaAddress>>39) & 0x1FF));
	if(!(pml4_val & 0x1))
	{
		createUserPDTPTable(v_userPml4Table, vmaAddress, oldPhysAddress);
	}
	checkUserPDTable(v_userPml4Table, vmaAddress, oldPhysAddress);
}

void createUserPDTPTable(uint64_t* v_userPml4Table, uint64_t vmaAddress, uint64_t oldPhysAddress) {

	uint64_t* userPdtpTable = (uint64_t*)getPage();
    	struct PAGE* p = getPageStruct((uint64_t)userPdtpTable);
    	p->use_cnt = 1;
	uint64_t* v_userPdtpTable = (uint64_t*)(VIRTUAL_BASE + (uint64_t)userPdtpTable);
	*(v_userPml4Table + ((vmaAddress>>39) & 0x1FF)) = ALL_ZERO | (((uint64_t)userPdtpTable & GET_40_BITS)) | 0x007;
    	for(int i = 0; i < PAGEINDEX; i++) {
		*(v_userPdtpTable+i) = 0x0;
	}
}

void checkUserPDTable(uint64_t* v_userPml4Table, uint64_t vmaAddress, uint64_t oldPhysAddress)
{
	uint64_t pml4_val = *(v_userPml4Table + ((vmaAddress>>39) & 0x1FF));
	uint64_t pdtp_base = pml4_val & GET_40_BITS;
	uint64_t* pdtp_val_ptr = (uint64_t*)(VIRTUAL_BASE + pdtp_base + (((vmaAddress>>30) & 0x1FF) * sizeof(uint64_t)));
	uint64_t pdtp_val = (uint64_t)(*pdtp_val_ptr);	
	if(!(pdtp_val & 0x1))
	{
		createUserPDTable(v_userPml4Table, vmaAddress, oldPhysAddress);
	}
	checkUserPTTable(v_userPml4Table, vmaAddress, oldPhysAddress);
}

void createUserPDTable(uint64_t* v_userPml4Table, uint64_t vmaAddress, uint64_t oldPhysAddress) {

	uint64_t* userPdTable = (uint64_t*)getPage();
    	struct PAGE* p = getPageStruct((uint64_t)userPdTable);
    	p->use_cnt = 1;
	uint64_t* v_userPdTable = (uint64_t*)(VIRTUAL_BASE + (uint64_t)userPdTable);
	uint64_t pml4_val = *(v_userPml4Table + ((vmaAddress>>39) & 0x1FF));
	uint64_t pdtp_base = pml4_val & GET_40_BITS;
	uint64_t* v_userPdtpTable = (uint64_t*)(VIRTUAL_BASE + pdtp_base + (((vmaAddress>>30) & 0x1FF) * sizeof(uint64_t)));
	*(v_userPdtpTable) = ALL_ZERO | (((uint64_t)userPdTable & GET_40_BITS)) | 0x007;
	for(int i = 0; i < PAGEINDEX; i++) {
		*(v_userPdTable+i) = 0x0;
	}
}

void checkUserPTTable(uint64_t* v_userPml4Table, uint64_t vmaAddress, uint64_t oldPhysAddress)
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
		createUserPTTable(v_userPml4Table, vmaAddress, oldPhysAddress);
	}
	else
	{
		checkUserEntry(v_userPml4Table, vmaAddress, oldPhysAddress);
	}
}

void createUserPTTable(uint64_t* v_userPml4Table, uint64_t vmaAddress, uint64_t oldPhysAddres) {

	uint64_t* userPtTable = (uint64_t*)getPage();
    	struct PAGE* p = getPageStruct((uint64_t)userPtTable);
    	p->use_cnt = 1;
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

	if(oldPhysAddres)
	{
		*(v_userPtTable + ((vmaAddress>>12) & 0x1FF)) = ALL_ZERO | ((oldPhysAddres & GET_40_BITS)) | 0x007;
        	struct PAGE* p = getPageStruct(oldPhysAddres & GET_40_BITS);
        	p->use_cnt += 1;
	}
	else
	{
		uint64_t phys_add = getPage();
        	struct PAGE* p = getPageStruct(phys_add);
        	p->use_cnt = 1;
	    	*(v_userPtTable + ((vmaAddress>>12) & 0x1FF)) = ALL_ZERO | ((phys_add & GET_40_BITS)) | 0x007;
	}
}

void checkUserEntry(uint64_t* v_userPml4Table, uint64_t vmaAddress, uint64_t oldPhysAddress)
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
            	struct PAGE* p = getPageStruct(phys_add);
            	p->use_cnt = 1;
		*(pt_val_ptr) = ALL_ZERO | ((phys_add & GET_40_BITS)) | 0x007;
	}
	else
	{	
		if(oldPhysAddress)
		{
			*(pt_val_ptr) = ALL_ZERO | ((oldPhysAddress & GET_40_BITS)) | 0x007;
            		struct PAGE* p = getPageStruct(oldPhysAddress & GET_40_BITS);
            		p->use_cnt += 1;
		}
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
		//uint64_t* v_pt_val = (uint64_t*) (VIRTUAL_BASE + (pt_val & GET_40_BITS));
		memcpy((void*)vmaAddress, (void*)vAddress, len);
	}
}

int user_page_exist(uint64_t pml4Address, uint64_t vAddress)
{
	uint64_t* v_userPml4Table = (uint64_t*)(VIRTUAL_BASE + pml4Address);
	uint64_t pml4_val = *(v_userPml4Table + ((vAddress>>39) & 0x1FF));
	if(!(pml4_val & 0x01))
		return 0;
	uint64_t pdtp_base = pml4_val & GET_40_BITS;
	uint64_t* pdtp_val_ptr = (uint64_t*)(VIRTUAL_BASE + pdtp_base + (((vAddress>>30) & 0x1FF)* sizeof(uint64_t)));
	uint64_t pdtp_val = (uint64_t)(*pdtp_val_ptr);
	if(!(pdtp_val & 0x01))
		return 0;
	uint64_t pd_base = pdtp_val & GET_40_BITS;
	uint64_t* pd_val_ptr = (uint64_t*)(VIRTUAL_BASE + pd_base + (((vAddress>>21) & 0x1FF) * sizeof(uint64_t)));
	uint64_t pd_val = (uint64_t)(*pd_val_ptr);
	if(!(pd_val & 0x01))
		return 0;
	uint64_t pt_base = pd_val & GET_40_BITS;
	uint64_t* pt_val_ptr = (uint64_t*)(VIRTUAL_BASE + pt_base + (((vAddress>>12) & 0x1FF) * sizeof(uint64_t)));
	uint64_t pt_val = (uint64_t)(*pt_val_ptr);
	if(pt_val & 0x01)
        	return 1;
    	return 0;
}

void useExistingPage(uint64_t pml4Address, uint64_t vAddress, uint64_t oldPhysAddress)
{   
	walkUserPageTables(pml4Address, vAddress, oldPhysAddress);
	flushTLB();
}

uint64_t* getPTTableEntry(uint64_t pml4Address, uint64_t vAddress)
{
	uint64_t* v_userPml4Table = (uint64_t*)(VIRTUAL_BASE + pml4Address);
	uint64_t pml4_val = *(v_userPml4Table + ((vAddress>>39) & 0x1FF));
	if(!(pml4_val & 0x01))
		return NULL;
	uint64_t pdtp_base = pml4_val & GET_40_BITS;
	uint64_t* pdtp_val_ptr = (uint64_t*)(VIRTUAL_BASE + pdtp_base + (((vAddress>>30) & 0x1FF)* sizeof(uint64_t)));
	uint64_t pdtp_val = (uint64_t)(*pdtp_val_ptr);	
	if(!(pdtp_val & 0x01))
		return NULL;
	uint64_t pd_base = pdtp_val & GET_40_BITS;
	uint64_t* pd_val_ptr = (uint64_t*)(VIRTUAL_BASE + pd_base + (((vAddress>>21) & 0x1FF) * sizeof(uint64_t)));
	uint64_t pd_val = (uint64_t)(*pd_val_ptr);
	if(!(pd_val & 0x01))
		return NULL;
	uint64_t pt_base = pd_val & GET_40_BITS;
	uint64_t* pt_val_ptr = (uint64_t*)(VIRTUAL_BASE + pt_base + (((vAddress>>12) & 0x1FF) * sizeof(uint64_t)));
    return pt_val_ptr;
}


void freeEntry(uint64_t val) {
	struct PAGE* p = getPageStruct(val & GET_40_BITS);
    p->use_cnt -= 1;
    if(p->use_cnt == 0) {
    	freePage(p);
    }
}

void flushPageTable(uint64_t pml4Address) {
    for (uint64_t pml4 = 0; pml4 < 511; pml4++) {
        
        uint64_t* pml4_ptr = (uint64_t*) (VIRTUAL_BASE + pml4Address + pml4*sizeof(uint64_t));
        uint64_t pml4_val = (uint64_t)(*pml4_ptr);
        if (pml4_val & 0x01) {
            for (uint64_t pdpe = 0; pdpe < 511; pdpe++) {
                
                uint64_t pdpe_base = (pml4_val & GET_40_BITS);
                uint64_t* pdpe_val_ptr = (uint64_t*)(pdpe_base + VIRTUAL_BASE + pdpe*sizeof(uint64_t));
                uint64_t pdpe_val = (uint64_t)(*pdpe_val_ptr);
                if (pdpe_val & 0x01) {
                    for (uint64_t pde = 0; pde < 512; pde++) {
                        
                        uint64_t pde_base = (pdpe_val & GET_40_BITS);
		                uint64_t* pde_val_ptr = (uint64_t*)(pde_base + VIRTUAL_BASE + pde*sizeof(uint64_t));
		                uint64_t pde_val = (uint64_t)(*pde_val_ptr);
                        if (pde_val & 0x01) {

                            for (uint64_t pte = 0; pte < 512; pte++) {
                                uint64_t pte_base = (pde_val & GET_40_BITS);
				                uint64_t* pte_val_ptr = (uint64_t*)(pte_base + VIRTUAL_BASE + pte*sizeof(uint64_t));
				                uint64_t pte_val = (uint64_t)(*pte_val_ptr);
                                if (pte_val & 0x01) {
						        	freeEntry(pte_val);
						        	*pte_val_ptr = 0UL;
                                }
                            }
                            freeEntry(pde_val);
                            *pde_val_ptr = 0UL;
                        }
                    }
                    freeEntry(pdpe_val);
                    *pdpe_val_ptr = 0UL;
                }
            }
            freeEntry(pml4_val);
            *pml4_ptr = 0UL;
        }
    }
flushTLB();
}
