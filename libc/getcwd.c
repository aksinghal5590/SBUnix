#include <libc.h>
#include <unistd.h>

char* getcwd(char* buf, size_t size) {
   return (char*)syscall2(__NR_getcwd, (void*)buf, size);     
}
