#include <stdlib.h>
#include <libc.h>
#include <unistd.h>
#include <stdio.h>

int main(int argc, char *argv[], char *envp[]) {
	printf("Hello: Welcome to our Shell\n");
	do {
		int pid;
		pid = fork();
		printf("%d\n", pid);
		if (pid == 0) {
			printf("C\n");
			execvpe("/bin/sbush", NULL, NULL);
		} else {
			printf("P\n");
			while(waitpid(pid, NULL, 0)) {
	        		yield();
	        	}
		}
	} while(1);
	return 1;
}
