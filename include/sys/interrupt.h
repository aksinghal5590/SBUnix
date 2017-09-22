#ifndef __INTERRUPT_H
#define __INTERRUPT_H

#include <sys/defs.h>

struct idt_table {
	uint16_t offset1;
	uint16_t sel;
	uint8_t ist;
	uint8_t flags;
	uint16_t offset2;
	uint32_t offset3;
 	uint32_t always0;
}__attribute__((packed));


struct idt_ptr {
	uint16_t limit;
	uint64_t baseptr;
}__attribute__((packed));


struct regs {
	uint64_t rip, cs, eflags, rsp, ss;
}__attribute__((packed));


void initInterrupts();


void initPIC();
void initIdt();
void loadIdt(struct idt_ptr idtptr);
void setVector(int pos, uint64_t baseptr, uint16_t sel, uint8_t flags);


extern void irq0();
extern void irq1();

void irq0Handler(struct regs *r);
void irq1Handler(struct regs *r);

void initKeyboard();
void initScanCodeTable();
void keyboardImpl();
void printDefault(uint8_t c);


void outIO(uint16_t port, uint8_t value);
uint8_t inIO(uint16_t port);

#endif
