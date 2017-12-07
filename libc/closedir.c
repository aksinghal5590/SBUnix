#include <dirent.h>
#include <unistd.h>
#include <stdlib.h>

int closedir(DIR *dirp) {
	int fd = dirp->fd;
//TODO	free((void*)dirp);
	return close(fd);
}
