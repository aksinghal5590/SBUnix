#ifndef VFS_H
#define VFS_H

#include "sys/defs.h"
#include "sys/tarfs.h"

#define MAX_LEN 1024

enum flags {
	O_RDONLY, O_RDWR, O_DIRECTORY
};

typedef struct vfs_dir DIR;


struct d_entry {
	char name[MAX_LEN];
	struct d_entry *parent;
	int inode_no;
};

struct inode {
	int inode_no;
	struct file *f;
};

struct vfs_file {
	int inode_no;
	int offset;
};

struct vfs_dir {
	struct d_entry *d_e;
	struct d_entry *d_e_child[64];
	int child_count;
	int curr_child;
};

struct dirent {
  char d_name[MAX_LEN];
};

void init_vfs();

void set_cwd(struct d_entry *new_cwd);

struct d_entry* get_cwd();

int update_fd_table(struct vfs_file *vf);

struct vfs_file* get_fd_table_entry(int fd);

void clear_fd_table_entry(int fd);

int sys_getdents(int fd, char *buf, int count);

int sys_open(char *path, int flags);

int sys_close(int fd);

int sys_chdir(char *path);

void sys_getcwd(char *buf, int size);

int sys_read(int fd, char *buf, int count);

int sys_write(int fd, char *buf, int count);

DIR* sys_opendir(const char *name);

struct dirent* sys_readdir(DIR *dirp);

int sys_closedir(DIR *dirp);


#endif
