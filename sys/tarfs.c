#include "sys/tarfs.h"
#include "sys/kprintf.h"
#include "sys/string.h"

uint32_t oct_to_dec(int num);
uint32_t pow(int b, int pow);
uint32_t atoi(char *str);

void* read_tarfs(char *file_name) {

    struct posix_header_ustar* start = (struct posix_header_ustar*)&_binary_tarfs_start;
    struct posix_header_ustar* end = (struct posix_header_ustar*)&_binary_tarfs_end;
    int size;
    
    while(start < end) {
    	size = oct_to_dec(atoi(start->size));

    	kprintf("%s\n", (start)->name);
        char* ptr = (char *)(start+1);

    	if(strcmp(file_name, (start)->name) == 0) {
    		kprintf("%s\n", "Found file");
    		return (void*)ptr;
    	}
    	if (size > 0) {
            start += size / (sizeof(struct posix_header_ustar) + 1) + 2;
        } else {
            start += 1;
        } 
    }

	return NULL;
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


uint32_t pow(int b, int pow)
{ 
    int result = 1;

    for(int i = 0; i < pow; ++i) {
        result *= b;
    }

    return result;
}

uint32_t atoi(char *str)
{
    int result = 0;
    // int s;

    // if (str[0] == '-') {
    //     s = -1;
    // } else if (str[0] == '+') {
    //     s = 1;
    // }   
    
    for (int i = 0; str[i] != '\0'; ++i)
        result = result*10 + str[i] - '0';

    return result;
}
