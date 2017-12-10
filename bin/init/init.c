#include <stdlib.h>

int main(int argc, char **argv)
{
    int pid;
    pid = fork();
    if (pid == 0) {
        execvpe("bin/sbush", NULL, NULL);
    } else {
        waitpid(pid, NULL, 0);
    }
    return 1;
}