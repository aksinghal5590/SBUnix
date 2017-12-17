#define _GNU_SOURCE
#include <libc.h>
#include <stdio.h>
#include <string.h>
#include <sys/defs.h>
#include <stdlib.h>
#include <unistd.h>

#define MAX_LENGTH 256
#define MAX_CMD 10

void performExpansion(char* input);
void performOperation(char* input, char* envp[]);
void performCDOperation(char* commandArg);
void addBinaryPath(char* command);
void checkScript(char *command, char *envp[]);

char commandArg[MAX_CMD][MAX_LENGTH];
char input[MAX_LENGTH];
char _exit[5] = "exit";
char* test[MAX_CMD];

int main(int argc, char *argv[], char *envp[]) {
	while(1) {
		printf("sbush$");
		gets(input);
		if(strcmp(_exit, input) == 0)
		{
			return 0;
		}
		performOperation(input, envp);
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
	char pwd[256];
	printf("PWD changed to: %s\n", getcwd(pwd, 256));
}

void checkScript(char *command, char *envp[]) {
	if(strlen(command) < 2) {
		printf("Please provide valid input!\n");
		exit(0);
	}
	if(command[0] == '.' && command[1] == '/') {
		command += 2;
		char cwd[256];
		getcwd(cwd, 256);
		strcat(cwd, command);
		command = cwd;
		int fd = open(command, O_RDONLY);
		if(fd > 0) {
			int i;
			do {
				char fileInput[MAX_LENGTH] = "";
				int len = 0;
				for(i = fgetc(fd); i != '\n' && i != EOF; i = fgetc(fd)) {
					fileInput[len++] = i;
				}
				if(len && fileInput[0] != '#') {
					fileInput[len] = '\0';
					performOperation(fileInput, envp);
				}

			} while(i != EOF);
			close(fd);
		}
	exit(0);
	}
}

void performOperation(char* input, char *envp[])
{       
	char command[1024] = "";
        int argVal = 0, len = 0, i = 0;
	int testcount = 0;
	int strlength = strlen(input);
	//int backgroundProcess = 0;

	while((i < strlength) && (input[i] != '\0') && (input[i] != ' '))
	{
		command[len++] = input[i++];	
	}
	command[len] = '\0';

	checkScript(command, envp);

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
				//backgroundProcess = 1;
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
		performCDOperation(commandArg[0]);
		return;
	}
	//add binary path to command if not already present
	addBinaryPath(command);

	if(argVal)
		testcount = argVal+2;
	else
		testcount = 3;

	test[0] = command;
	if(argVal) {
		for(int j = 1; j < testcount-1; j++)
			test[j] = commandArg[j-1];
	}
	else {
		test[1] = NULL;
	}
	test[testcount-1] = (char*) NULL;
	printf("%s\n", test[0]);

	pid_t pid = fork();
	if(pid > 0) {
        	//int status;
		//int ret = 0;
                //if(!backgroundProcess) { //Parent process will not wait for child process in case of background process 
		//	ret = waitpid(pid,&status, 0);
		//}
		//if(ret) {
			yield();
			exit(0);
		//} 
	} else if(pid == 0) {
		//printf("%s\n", test[0]);
		exit(execvpe(test[0], test, NULL));
		//int err = execvpe("/bin/ls", NULL, NULL);
	}
}

void addBinaryPath(char* command) {

	if(command != NULL)
	{
		if(command[0] == '/')
			return;
		char *bin_path = "/bin/";
		strcat(bin_path, command);
		strcpy(command, bin_path);
	}
}
