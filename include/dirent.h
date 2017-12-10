#ifndef _DIRENT_H
#define _DIRENT_H

#define NAME_MAX 255

struct dirent {
 char d_name[NAME_MAX+1];
};

struct vfs_dir {
	int fd;
        struct dirent *dir_child[64];
        int child_count;
        int curr_child;
	int read;
};

typedef struct vfs_dir DIR;

DIR *opendir(const char *name);
struct dirent *readdir(DIR *dirp);
int closedir(DIR *dirp);

#endif
