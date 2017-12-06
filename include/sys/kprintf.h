#ifndef __KPRINTF_H
#define __KPRINTF_H

#include <stdarg.h>
#include <sys/defs.h>

void kprintf(const char *fmt, ...);
void parsefmt(char *kstring, const char *fmt, va_list val);
void convert(uint64_t a, char* ch, int base);
void memshift();
void printtimer(uint64_t a);
void printkeyboard(char *s);

void* memset(void *ptr, int value, size_t num);
void* memcpy(void* dest, const void* src, size_t n);
uint64_t* kmalloc(int64_t size);

void loadKernel(uint64_t physbase, uint64_t physfree);

#endif
