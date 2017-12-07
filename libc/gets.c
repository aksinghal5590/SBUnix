#include <stdio.h>
#include <string.h>
#include <unistd.h>

char* gets(char *s) {
	int len = 0;
	char ch = 'a';
	while(ch != '\n' && ch != EOF && ch != '\0') {
		int size = read(0, &ch, 1);
		if(size == 0)
			break;
		s[len++] = ch;
	}
	s[len] = '\0';
	return s;
}
