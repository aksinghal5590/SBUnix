#include <stdlib.h>
#include <libc.h>
#include <unistd.h>

int main(int argc, char **argv)
{	write(1, "dfdfdgfg", 20);
	while(1) {
		int pid;
	    pid = fork();
	    if (pid == 0) {
	        execvpe("bin/sbush", NULL, NULL);
	    } else {
	        waitpid(pid, NULL, 0);
	    }
	}	

	return 1;
}