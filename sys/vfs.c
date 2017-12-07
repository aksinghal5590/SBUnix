#include "sys/vfs.h"
#include "sys/pcb.h"
#include "sys/tarfs.h"
#include "sys/string.h"
#include "sys/kprintf.h"
#include "sys/terminal.h"

extern struct PCB *current_proc;

struct d_entry* get_d_entry_parent(int pos, char *f_name);
void print_vfs();

struct d_entry d_entries[256];
struct inode inodes[256];
int d_e_count = 0;
int inode_count = 0;

void init_vfs() {

	struct file *f = get_tarfs_list();
	if(f == NULL) {
		init_tarfs();
		f = get_tarfs_list();
	}
	while(f->next != NULL) {

		inodes[inode_count].inode_no = inode_count;
		inodes[inode_count].f = f;

		strcpy(d_entries[d_e_count].name, f->name);
		d_entries[d_e_count].parent = get_d_entry_parent(d_e_count, f->name);
		d_entries[d_e_count].inode_no = inode_count;

		d_e_count++;
		inode_count++;
		f = f->next;
	}
//	print_vfs();
}

struct d_entry* get_cwd() {
	return current_proc->cwd;
}

void set_cwd(struct d_entry *new_cwd) {
	current_proc->cwd = new_cwd;
}

int update_fd_table(struct vfs_file *vf) {
	int ret;
	current_proc->fd_table[current_proc->fd_count] = vf;
	ret = current_proc->fd_count++;
	return ret;
}

struct vfs_file* get_fd_table_entry(int fd) {
	if(fd < 0)
		return NULL;
	return current_proc->fd_table[fd];
}

void clear_fd_table_entry(int fd) {
	if(fd < 0)
		return;
	current_proc->fd_table[fd] = NULL;
}

void print_vfs() {
	for(int i = 1; i < d_e_count; i++) {
		kprintf("Name: %s, Parent: %s, Inode: %d\n", d_entries[i].name, d_entries[i].parent->name, d_entries[i].inode_no);
	}
	for(int i = 0; i < inode_count; i++) {
//		kprintf("Inode: %d, Type: %s, Addr : %x\n", inodes[i].inode_no, inodes[i].f->type, inodes[i].f->addr);
	}
}

struct d_entry* get_d_entry_parent(int pos, char *f_name) {

	if(pos == 0) {
		return NULL;
	}
	int i = strlen(f_name) - 1;
	if(f_name[i] == '/') {
                i--;
        }
        while(i >= 0 && f_name[i] != '/') {
                i--;
        }
        int j;
	char name[1024];
        for(j = 0; j <= i; j++) {
                name[j] = f_name[j];
        }
        name[j] = '\0';
	for(j = 0; j < pos; j++) {
		if(strcmp(d_entries[j].name, name) == 0)
			return &d_entries[j];
	}
	return NULL;
}


int sys_getdents(int fd, char *buf, int count) {
	if(fd < 0)
		return -1;
	if(get_fd_table_entry(fd) == NULL)
		return -1;
	int inode_no = get_fd_table_entry(fd)->inode_no;
	struct d_entry *dir = NULL;
	for(int i = 0; i < d_e_count; i++) {
		if(inode_no == d_entries[i].inode_no) {
			dir = &d_entries[i];
			break;
		}
	}
	int dirent_size = sizeof(struct dirent);
	int ret = 0;
	for(int i = 0; i < d_e_count && ret <= count; i++) {
		if(dir == d_entries[i].parent) {
			struct dirent *dir_ent = (struct dirent*)kmalloc(dirent_size);
			strcpy(dir_ent->d_name, d_entries[i].name);
			memcpy((void*)(buf + ret), (void*)dir_ent, dirent_size);
			ret += dirent_size;
		}
	}
	return ret;
}

int sys_open(char *path, int flags) {
	const struct file *f = fopen(path);
	int ret = -1;
	if(f != NULL) {
		for(int i = 0; i < inode_count; i++) {
			if(f == inodes[i].f) {
				struct vfs_file *vf = (struct vfs_file*)kmalloc(sizeof(struct vfs_file));
				vf->inode_no = i;
				vf->offset = 0;
				ret = update_fd_table(vf);
				return ret;
			}
		}
	}
	return ret;
}

int sys_close(int fd) {
	if(fd < 0)
		return -1;
	if(get_fd_table_entry(fd) == NULL)
		return -1;
	clear_fd_table_entry(fd);
	return 0;
}

int sys_chdir(char *path) {
	char *full_path = (char*)kmalloc(1024);
	char *case1 = ".";
	char *case2 = "./";
	char *case3 = "..";
	char *case4 = "../";
	if(path[0] == '/') {
		strcpy(full_path, path);
	} else if(!strstarts(path, case3)) {
		strcpy(full_path, get_cwd()->parent->name);
	} else {
		strcpy(full_path, get_cwd()->name);
	}
	if(!strstarts(path, case4)) {
		path += 3;
		strcat(full_path, path);
	} else if(!strstarts(path, case3)) {
		// do nothing
	} else if(!strstarts(path, case2)) {
		path += 2;
		strcat(full_path, path);
	} else if(!strstarts(path, case1)) {
		// do nothing
	} else {
		strcat(full_path, path);
	}
	if(full_path[strlen(full_path) - 1] != '/') {
		strcat(full_path, "/");
	}

	for(int i = 0; i < d_e_count; i++) {
		if(strcmp(full_path, d_entries[i].name) == 0) {
			set_cwd(&d_entries[i]);
			return 0;
		}
	}
	return -1;
}

void sys_getcwd(char *buf, int size) {
	size = (size < strlen(get_cwd()->name)) ? size : strlen(get_cwd()->name);
	memcpy(buf, get_cwd()->name, size);
	buf[size] = '\0';
}

int sys_read(int fd, char *buf, int count) {
	if(fd < 0)
		return -1;
	if(fd == 0)
		return read_stdin(buf, count);
	if(get_fd_table_entry(fd) == NULL)
		return -1;
	int bytes_read = fread(inodes[get_fd_table_entry(fd)->inode_no].f, buf, get_fd_table_entry(fd)->offset, count);
	get_fd_table_entry(fd)->offset += bytes_read;
	return bytes_read;
}

int sys_write(int fd, char *buf, int count) {
	if(fd < 1)
		return -1;
	char* wr = (char*)kmalloc(count + 1);
	if(fd == 1) {
		memcpy((void*)wr, (void*)buf, count);
		if(wr[count] != '\0')
			wr[++count] = '\0';
		return write_stdout(wr, count);
	}
	if(fd == 2) {
		memcpy((void*)wr, (void*)buf, count);
		if(wr[count] != '\0')
			wr[++count] = '\0';
		return write_stderr(wr, count);
	}
	kprintf("Write not supported!!\n");
	return -1;	
}

DIR* sys_opendir(const char *name) {
	if(name == NULL)
		return NULL;
	char *full_name = (char*)kmalloc(1020);
	if(name[0] != '/') {
		sys_getcwd(full_name, 1020);
		strcat(full_name, name);
	} else {
		strcpy(full_name, name);
	}
	if(full_name[strlen(full_name) - 1] != '/') {
		strcat(full_name, "/");
	}
	for(int i = 0; i < d_e_count; i++) {
		if(strcmp(full_name, d_entries[i].name) == 0) {
			DIR *dir = (DIR*)kmalloc(sizeof(DIR));
			dir->d_e = &d_entries[i];
			dir->child_count = 0;
			dir->curr_child = 0;
			for(int j = i; j < d_e_count; j++) {
				if(dir->d_e == d_entries[j].parent) {
					dir->d_e_child[dir->child_count++] = &d_entries[j];
				}
			}
			return dir;
		}
	}
	return NULL;
}

struct dirent* sys_readdir(DIR *dirp) {
	if(dirp == NULL)
		return NULL;
	struct dirent *dir = (struct dirent*)kmalloc(sizeof(struct dirent));
	if(dirp->curr_child < dirp->child_count) {
		strcpy(dir->d_name, dirp->d_e_child[dirp->curr_child++]->name);
		return dir;
	}
	return NULL;
}

int sys_closedir(DIR *dirp) {
	if(dirp == NULL)
		return -1;
	dirp = NULL;
	return 0;
}
