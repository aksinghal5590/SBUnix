#ifndef VFS_H
#define VFS_H

#include "sys/defs.h"
#include "sys/tarfs.h"

#define MAX_LEN 255
#define O_RDONLY 00000000
#define O_DIRECTORY 00200000

struct d_entry {
	char name[MAX_LEN + 1];
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

struct dirent {
  char d_name[MAX_LEN + 1];
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

#endif
