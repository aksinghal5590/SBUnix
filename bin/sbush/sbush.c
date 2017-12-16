#define _GNU_SOURCE
#include <libc.h>
#include <stdio.h>
#include <string.h>
#include <sys/defs.h>
#include <stdlib.h>
#include <unistd.h>

#define MAX_LENGTH 512
#define MAX_BUF_LENGTH 512*10
#define MAX_CMD 10

void performExpansion(char* input);
void performOperation(char* input, char* envp[]);
void performCDOperation(char* commandArg);
void addBinaryPath(char* command);

int main() {
	int argc = 1;
	char *argv[] = {NULL};
	char *envp[] = {"PWD=/", "PATH:/bin", NULL};
	printf("Hello: Welcome to our Shell\n");
	char input[MAX_LENGTH];
	int len = 0;
	char exit[5] = "exit";
	if(argc > 1)
	{
		//puts("Entering file mode");
		for(int i = 1; i < argc; i++)
		{
			char* filename = argv[i];
			int fd = open(filename, O_RDONLY);
			int j;
			do
			{
				char fileInput[MAX_LENGTH] = "";
				len = 0;
				for(j = fgetc(fd); j != '\n' && j != EOF; j = fgetc(fd))
				{
					fileInput[len++] = j;
				}
				if(len && fileInput[0] != '#')
				{
					fileInput[len] = '\0';

					performOperation(fileInput, envp);
                                        
				}
			}
			while(j != EOF);
			close(fd);
		}
	}
	else {
		while(1) {
			len = 0;
			gets(input);
			if(strcmp(exit, input) == 0)
			{
				return 0;
			}
			performOperation(input, envp);
		}
	}
	return 0;
}

void performCDOperation(char* commandArg)
{
	int res = chdir(commandArg);
	if(res < 0)
	{
		char err[] = "Error in performing cd operation. Please check the path provided in cd.\n";
		int res = write(1, err, strlen(err));
		if(res == -1) return;
	}
}

void performOperation(char* input, char* envp[])
{       
	char command[1024] = "";
	char commandArg[MAX_CMD][MAX_LENGTH];
        int argVal = 0, testcount = 0, len = 0, i = 0;
	int strlength = strlen(input);
	int backgroundProcess = 0;

	while((i < strlength) && (input[i] != '\0') && (input[i] != ' '))
	{
		command[len++] = input[i++];	
	}
	command[len] = '\0';
	while(i < strlength)
	{
		int length = 0;
		int present = 0;
		while((i < strlength) && (input[i] != '\0') && (input[i] != ' '))
		{
			commandArg[argVal][length++] = input[i++];
			present = 1;
		}
		if(present)
		{
			if(length == 1 && commandArg[argVal][0] == '&')
			{
				commandArg[argVal][0] = '\0'; //making the arg empty
				backgroundProcess = 1;
			}
			else
			{
				commandArg[argVal][length] = '\0';
				argVal++;
			}
		}
		else
			i++;
	}
	//check for built-in[cd, exit] or check binary[ls, cat]
	if(strcmp(command,"cd") == 0)
	{
		char temp[50];
		performCDOperation(commandArg[0]);
		printf("New current directory: %s\n", getcwd(temp, 50));
		return;
	}
	if(argVal)
		testcount = argVal+2;
	else
		testcount = 3;

	//add binary path to command if not already present
	addBinaryPath(command);

	char* test[testcount];
	test[0] = command;
	if(argVal) {
		for(int j = 1; j < testcount-1; j++)
			test[j] = commandArg[j-1];
	}
	else {
		test[1] = NULL;
	}
	test[testcount-1] = (char*) NULL;
        printf("test 0: %s\n", test[0]);

	pid_t pid = fork();
	if(pid > 0) {
        	int status;
                if(!backgroundProcess) { //Parent process will not wait for child process in case of background process 
			printf("parent: %x\n", &test[0]);
			waitpid(pid,&status, 0);
			yield();
		}     
	} else if(pid == 0) {
		// int err = execvpe(test[0], test, envp);
		// printf("child: test 0 - %s\n", test[0]);
		int err = execvpe("/bin/ls", NULL, NULL);
		char errStr[] = "Error in running command\n";
		if(err == -1)
	        {
                   err = write(1, errStr,strlen(errStr));
                   exit(1);
                }
                exit(0);
	}
}

void addBinaryPath(char* command) {

	if(command != NULL)
	{
		if(command[0] == '/')
			return;
		char bin_path[MAX_LENGTH];
		getcwd(bin_path, MAX_LENGTH);
		strcat(bin_path, command);
		strcpy(command, bin_path);
		//puts("BINARY_PATH");
		//puts(BINARY_PATH);
		//puts("Command");
		//puts(command);
	}
}
