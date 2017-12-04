#ifndef _TARFS_H
#define _TARFS_H

#include "sys/defs.h"

extern char _binary_tarfs_start;
extern char _binary_tarfs_end;

struct posix_header_ustar {
  char name[100];
  char mode[8];
  char uid[8];
  char gid[8];
  char size[12];
  char mtime[12];
  char checksum[8];
  char typeflag[1];
  char linkname[100];
  char magic[6];
  char version[2];
  char uname[32];
  char gname[32];
  char devmajor[8];
  char devminor[8];
  char prefix[155];
  char pad[12];
};

struct file {
  int fd;
  char name[1024];
  uint64_t addr;
  char type[1];
  int size;
  uint64_t offset;
  struct file *next;
};

void init_tarfs();

void* read_tarfs(char*s);

int fopen(char *name);

int fread(int fd, char *buf, int count);

struct file* opendir(int fd);

int fclose(int fd);


#endif
