#include<libc.h>
#include<stdio.h>
#include<string.h>
#include<unistd.h>

int main(int argc, char* argv[], char* envp[]) {
	char output[256*BUF_SIZE];
	for(int k = 1; k < argc; k++) {
		if(strlen(output)) {
			strcat(output, " ");
		}
        	if(!strcmp(argv[k], "$PWD")) {
			char cwd[256];
			getcwd(cwd, 256);
			strcat(output, cwd);
        	} else {
        		strcat(output, argv[k]);
        	}
	}
	if(!strlen(output))
		strcpy(output, "");
	puts(output);
        return 0;
}
