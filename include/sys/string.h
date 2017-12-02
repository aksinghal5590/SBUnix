#ifndef _STRING_H
#define _STRING_H

#include <sys/defs.h>

void* memcpy(void* dest, const void* src, size_t n);
size_t strlen(const char *s);
int strcmp(const char *s, const char *t);
char* strcpy(char* d, const char* s);
char* strcat(char* d, const char* s);
int fgetc(int fd);

#endif
