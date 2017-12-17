#include <libc.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <dirent.h>

#define MAX_LEN 256

void executecat(int argc, char* argv[]);
void readWrite(int fd);

int main(int argc, char* argv[], char* envp[]) {
        executecat(argc, argv);
	return 0;
}

void executecat(int argc, char* argv[]) {

	int fd;

	if(argc == 1) {
		while(1) {
			char ch[1024];
        		while(read(0, ch, 1024) > 0) {
                		write(1, ch, 1024);
        		}
		}
	} else {
		for(int i = 1; i < argc; i++) {

			char fileName[MAX_LEN];
			if(argv[i][0] != '/') {
        			getcwd(fileName, MAX_LEN);
				strcat(fileName, argv[i]);
			} else {
				strcpy(fileName, argv[i]);
			}

			fd = open(fileName, O_RDONLY);
			if(fd < 0) {
				exit(1);
        		}
			readWrite(fd);
			close(fd);
		}
	}
}

void readWrite(int fd) {
	char ch = ' ';
	while(read(fd, &ch, 1) > 0) {
                write(1, &ch, 1);
        }
}
