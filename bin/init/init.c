#include <stdlib.h>
#include <libc.h>
#include <unistd.h>
#include <stdio.h>

int main(int argc, char **argv)
{	
	write(1, "dfdfdgfg", 20);
	// while(1) {
		int pid;
	    pid = fork();
	    printf("%d\n", pid);
	    if (pid == 0) {
	    	printf("%s\n", "ashyadav");
	        execvpe("/bin/sbush", NULL, NULL);
	    } else {
	        while(waitpid(pid, NULL, 0)) {
	        	printf("%s\n", "In init");
	        	yield();
	        }
	        // printf("%s\n", "jgkfgfkng");
	    }
	// }	
    while(1){ printf("%s", "out");};
	return 1;
}