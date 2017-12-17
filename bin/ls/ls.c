#define _GNU_SOURCE
#include <dirent.h>
#include <string.h>
#include <stdio.h>
#include <libc.h>
#include <unistd.h>
#define MAX_LEN 256

int list(char* fileName);


int main(int argc, char* argv[], char* envp[]) {

	char fileName[MAX_LEN];
	getcwd(fileName, MAX_LEN);
	if(argc == 1) {
		list(fileName);
	}

	if(argc > 1) {
		for(int i = 1; i < argc; i++) {
			if(argv[i][0] == '-') {
				puts("Not supported");
				continue;
			}
			list(argv[i]);
		}
	}
        return 0;
}

int list(char* fileName) {

	DIR *dirp = opendir(fileName);
	if(dirp != NULL) {
		struct dirent *entry = NULL;
		while((entry = readdir(dirp)) != NULL) {
			printf("%s\n", entry->d_name);
		}
		closedir(dirp);
	}
	return 0;
}
