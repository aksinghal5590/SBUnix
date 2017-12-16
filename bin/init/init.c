#include <stdlib.h>
#include <libc.h>
#include <unistd.h>

int main(int argc, char **argv)
{	write(1, "dfdfdgfg", 20);
	while(1) {
		int pid;
	    pid = fork();
	    if (pid == 0) {
		write(1, "In child", 15);
	        execvpe("bin/sbush", NULL, NULL);
	    } else {
		write(1, "In parent", 15);
	        waitpid(pid, NULL, 0);
	    }
	}	

	return 1;
}
