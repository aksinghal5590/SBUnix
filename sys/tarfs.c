#include "sys/tarfs.h"
#include "sys/kprintf.h"
#include "sys/string.h"

uint32_t oct_to_dec(int num);
uint32_t pow(int b, int pow);
uint32_t atoi(char *str);
void append_file(struct file *f);
void print_file_list();

struct file *f_list_head, *f_list_tail;

struct file* get_tarfs_list() {
	return f_list_head;
}

void init_tarfs() {
	struct posix_header_ustar* start = (struct posix_header_ustar*)&_binary_tarfs_start;
	struct posix_header_ustar* end = (struct posix_header_ustar*)&_binary_tarfs_end;
	struct file* f;
	char* root_name = "/";
	const uint64_t H_SIZE = sizeof(struct posix_header_ustar);
	uint64_t offset = 0;
	
	f = (struct file*)kmalloc(sizeof(struct file));
        strcpy(f->name, root_name);
	f->type[0] = '5';
        f->size = oct_to_dec(atoi(start->size));
	f->addr = 0;
        f->next = NULL;
        append_file(f);

	while(start < end && (strlen(start->name) > 0)) {
		f = (struct file*)kmalloc(sizeof(struct file));

		char* full_name = (char*)kmalloc(1024);
		strcpy(full_name, root_name);
		strcat(full_name, start->name);
		strcpy(f->name, full_name);

		f->type[0] = start->typeflag[0];
		f->size = oct_to_dec(atoi(start->size));
		f->addr = (uint64_t)(&_binary_tarfs_start + offset + H_SIZE);
		f->next = NULL;
		append_file(f);
		uint64_t size = oct_to_dec(atoi(start->size));
		uint64_t diff = 0;
		if (size > 0) {
			diff = ((size % H_SIZE) == 0) ? size : size + (H_SIZE - size%H_SIZE);
			start += size / (H_SIZE + 1) + 2;
		} else {
			start += 1;
		}
		offset += diff + H_SIZE;
	}
//	print_file_list();
}

void print_file_list() {
	struct file *temp = f_list_head;
	while(temp->next != NULL) {
		kprintf("File Name: %s   Size: %d\n", temp->name, temp->size);
		temp = temp->next;
	}
}

void* read_tarfs(char *file_name) {
	struct file *f = f_list_head;
        while(f != NULL) {
		if(strcmp(f->name, file_name) == 0) {
			return (void*)(f->addr);
		}
		f = f->next;
        }
        return NULL;
}

struct file* fopen(char *name) {
	struct file *f = f_list_head;
	while(f->next != NULL) {
		if(strcmp(f->name, name) == 0) {
			return f;
		}
		f = f->next;
	}
	return NULL;
}

int fread(struct file *f, char *buf, int offset, int count) {
	if(f != NULL) {
		count = ((f->size - offset) >= count) ? count : (f->size - offset);
		count = f->size >= count ? count : f->size;
		memcpy((void*)buf, (void*)(f->addr + offset), count);
		return count;
	}
	return 0;
}

uint32_t oct_to_dec(int num) {
    int result = 0;
    int cnt = 0;
    int r; 
 
    while (num != 0) { 
        r = num % 10; 
        num /= 10; 
        result += r * pow(8, cnt);
        ++cnt;
    }

    return result;
}


uint32_t pow(int b, int pow) { 
    int result = 1;
    for(int i = 0; i < pow; ++i) {
        result *= b;
    }
    return result;
}

uint32_t atoi(char *str) {
    int result = 0;
    for (int i = 0; str[i] != '\0'; ++i)
        result = result*10 + str[i] - '0';
    return result;
}

void append_file(struct file *f) {
	if(f_list_head == NULL) {
		f_list_head = f_list_tail = f;
	} else {
		f_list_tail->next = f;
		f_list_tail = f_list_tail->next;
	}
}
