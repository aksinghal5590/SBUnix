#include <sys/interrupt.h>
#include <sys/defs.h>
#include <sys/kprintf.h>
#include "sys/pcb.h"

extern void systemCallHandler();
extern void pageFaultHandler();
extern void updateUserCR3_Val(uint64_t userPml4Table);
extern void set_tss_rsp(void *rsp);
void pageFault();

extern struct PCB *userThread;
int t_flag = 0;

struct idt_table idtTable[256];

struct idt_ptr idtptr;

uint64_t ticks = 0;
uint64_t seconds = 0;

void initInterrupts() {
	
	initPIC();

	initKeyboard();

	initIdt();
}

void initPIC() {
	
	outIO(0x20, 0x11);
        outIO(0xA0, 0x11);
        outIO(0x21, 0x20);
        outIO(0xA1, 0x28);
        outIO(0x21, 0x04);
        outIO(0xA1, 0x02);
        outIO(0x21, 0x01);
        outIO(0xA1, 0x01);

        outIO(0x21, 0xFC);
        outIO(0xA1, 0xFF);
}

void setVector(int pos, uint64_t baseptr, uint16_t sel, uint8_t flags) {
	
	idtTable[pos].offset1 = (baseptr & 0x000000000000FFFF);
	idtTable[pos].offset2 = ((baseptr >> 16) & 0x000000000000FFFF);
	idtTable[pos].offset3 = ((baseptr >> 32) & 0x00000000FFFFFFFF);
	idtTable[pos].sel = sel;
	idtTable[pos].always0 = 0;
	idtTable[pos].flags = flags;
	idtTable[pos].ist = 0;
}

void loadIdt(struct idt_ptr idt_ptr) {
	__asm__
	(
		"lidt %0"
		:
		:"m"(idt_ptr)
	);
}

void initIdt() {
	
	idtptr.limit = sizeof(struct idt_table) * 256 - 1;
	idtptr.baseptr = (uint64_t)&idtTable;

	for(int i = 0; i < 256; i++) {
		setVector(i, (uint64_t)irqDefault, 0x08, 0x8E);
	}

	setVector(32, (uint64_t)irq0, 0x08, 0x8E);
	setVector(33, (uint64_t)irq1, 0x08, 0x8E);
	setVector(14, (uint64_t)pageFault, 0x08, 0x8E);	
	setVector(128, (uint64_t)systemCallHandler, 0x08, 0xEE);
	loadIdt(idtptr);

	__asm__("sti");
}

void irq0Handler(struct regs *r) {

	ticks++;
	if(ticks % 18 == 0) {
		seconds += 1;
		printtimer(seconds);
		ticks = 0;
	}
	outIO(0x20, 0x20);
}

void irq1Handler(struct regs *r) {

	keyboardImpl();

        outIO(0x20, 0x20);
}


void outIO(uint16_t port, uint8_t value) {	
	__asm__ volatile
	(
		"outb %1, %0 \n\t"
		:
		:"Nd"(port) , "a"(value)
	);
}

uint8_t inIO(uint16_t port) {
        uint8_t ret;
        __asm__ volatile
        (
                "in %1, %0 \n\t"
                :"=al"(ret)
                :"Nd"(port)
        );
	return ret;
}

void isr_handler(registers_t regSet)
{
    switch (regSet.int_number) {
    /*    case 0:
            divide_by_zero_handler(regSet);
            break;
        case 10:
            tss_fault_handler(regSet);
            break;
        case 13:
            gpf_handler(regSet);
            break;*/
        case 14:
            pageFaultHandler(regSet);
            break;
        default:
            break;
    }
}
