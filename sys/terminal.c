#include "sys/kprintf.h"
#include "sys/terminal.h"

char buffer[4000];
int buf_pos = 0;
static volatile int flush = 1;

void append_buffer(const char *s, int size) {

	memcpy((void*)(buffer + buf_pos), (void*)s, size);
	buf_pos += size;
	char *temp = (char*)s;
	if(temp[0] == '\0')
		temp[0] = '\n';
	write_stdout(temp, size);
}

void backspace() {
	if(buf_pos) {
		buf_pos--;
		backSpace();
	}
}

void flush_buffer() {
	flush = 1;
}

int read_stdin(char *buf, int size) {

	flush = 0;
	__asm__("sti");
	while(flush == 0);

	size = size < buf_pos ? size : buf_pos;
	memcpy((void*)buf, (void*)buffer, size);

	memset((void*)buffer, 0, 4000);
	buf_pos = 0;

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
	kprintf("Error: %s\n", buf);
	return size;
}
