#define _GNU_SOURCE
#include <string.h>
#include <stdio.h>
#include <libc.h>
#include <unistd.h>

int main(int argc, char* argv[], char* envp[]) {

	if(argc >= 3) {
		if((strlen(argv[1]) == 2) && (argv[1][0] == '-') && (argv[1][1] == '9'))
		{
			int pid;
		        int len = strlen(argv[2]);
			if(len == 1)
			{
				pid = (argv[2][0] - 48);
			}
			else if(len == 2)
			{
				pid = ((argv[2][0] - 48)*10) + (argv[2][1] - 48);
			}
			else
			{
				printf("Given pid by user not found. Please chekc.\n");
				return 0;
			}
			kill(pid);	
		}
		else
		{
			printf("kill command entered is not supported. Please check.\n");
			return 0;
		}
	}
	else
	{
		printf("Error in Kill command.\n");
	}
        return 0;
}
