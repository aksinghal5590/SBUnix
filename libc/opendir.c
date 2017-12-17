#include <dirent.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#define O_DIRECTORY 00200000
DIR dirp;

DIR* opendir(const char *name) {
	int flags = O_DIRECTORY;
	int len = strlen(name);
	char temp[256];
	for(int i = 0; name[i] != '\0'; i++) {
		temp[i] = name[i];
	}
	if(name[len - 1] != '/') {
		temp[len] = '/';
		len++;
	}
	temp[len] = '\0';
	printf("Open dir1\n");
	int fd = open(temp, flags);
	printf("Open dir2\n");
	if(fd > 0) {
		DIR *dir = &dirp;//TODO (DIR*)malloc(sizeof(DIR));
		dir->fd = fd;
		dir->child_count = 0;
		dir->curr_child = 0;
		dir->read = 0;
		return dir;
	} else {
		printf("Directory not found!!\n");
	}
	return NULL;
}
