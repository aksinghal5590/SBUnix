#include "sys/kprintf.h"
#include "sys/terminal.h"

char buffer[4000];
int buf_pos = 0;
char temp[100];

void append_buffer(const char *s, int size) {
	memcpy((void*)temp, (void*)s, size);
	temp[size] = '\0';
	kprintf("%s",temp);

	memcpy((void*)(buffer + buf_pos), (void*)s, size);
	buf_pos += size;
}

void flush_buffer() {
	memset((void*)buffer, 0, 4000);
	buf_pos = 0;
}

int read_stdin(char *buf, int size) {
	memcpy((void*)buf, (void*)buffer, size);
	flush_buffer();
	return size;	
}

int write_stdout(char *buf, int size) {
	if(buf[size] != '\0')
		buf[size] = '\0';
	kprintf("%s", buf);
	return size;
}

int write_stderr(char *buf, int size) {
	if(buf[size] != '\0')
                buf[size] = '\0';
	kprintf("Error: %s", buf);
	return size;
}
