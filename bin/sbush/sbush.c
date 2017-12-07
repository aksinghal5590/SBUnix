#include <stdlib.h>
#include <libc.h>
#include <unistd.h>


int main() {
  write(1, "OS by Akshat, Ashish and Vaibhav, version1", 20);
/*  yield();
  write(1, "OS by Akshat, Ashish and Vaibhav, version2", 20);
  yield();
  write(1, "OS by Akshat, Ashish and Vaibhav, version3", 20);
  char buf[1024];
  getcwd(buf, 1024);
  write(1, buf, 1024);
  chdir("bin");
  getcwd(buf, 1024);
  write(1, buf, 1024);*/
  while(1)
  {
    ;
  }
  return 0;
}
