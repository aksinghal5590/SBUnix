#include <libc.h>
#include <dirent.h>
#include <unistd.h>
#include <stdlib.h>

#define MAXLEN 256*25

struct dirent *readdir(DIR *dirp) {
	if(dirp == NULL)
		return NULL;
	char buf[MAXLEN];
	if(!dirp->read) {
		int bytesread = getdents(dirp->fd, buf, MAXLEN);
		for(int i = 0; i < bytesread;) {
			dirp->dir_child[dirp->child_count++] = (struct dirent*)(buf + i);
			i += sizeof(struct dirent);
		}
		dirp->read = 1;
	}
	if(dirp->curr_child < dirp->child_count) {
		return dirp->dir_child[dirp->curr_child++];
	}
	return NULL;
}
