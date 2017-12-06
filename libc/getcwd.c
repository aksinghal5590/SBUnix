#include <libc.h>
#include <unistd.h>

int getcwd(char* buf, size_t size) {
   return syscall2(__NR_getcwd, (void*)buf, size);     
}
