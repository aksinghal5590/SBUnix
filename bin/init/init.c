#include <stdlib.h>
#include <libc.h>
#include <unistd.h>
#include <stdio.h>

int main(int argc, char *argv[], char *envp[]) {
	int pid = fork();
	if (pid == 0) {
		execvpe("/bin/sbush", NULL, NULL);
	} else {
	       	yield();
	}
	return 0;
}
