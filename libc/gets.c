#include <stdio.h>
#include <string.h>
#include <unistd.h>

char* gets(char *s) {
	int len = 1024;
	int size = read(0, s, len);
	s[size] = '\0';
	return s;
}
