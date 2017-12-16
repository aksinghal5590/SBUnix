#include <stdlib.h>
#include <libc.h>
#include <unistd.h>
#include <stdio.h>

int main(int argc, char *argv[], char *envp[]) {
	do {
		int pid;
		pid = fork();
		printf("%d\n", pid);
		if (pid == 0) {
			execvpe("/bin/sbush", NULL, NULL);
		} else {
			while(waitpid(pid, NULL, 0)) {
	        		yield();
	        	}
		}
	} while(1);
	return 1;
}
