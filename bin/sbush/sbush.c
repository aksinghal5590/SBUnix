#include <stdlib.h>
#include <libc.h>
#include <unistd.h>
#include <stdio.h>

int a[20];

int main() {

  for(int i = 0; i < 20; i++) {
	a[i] = 0;
	//printf("Value of a[%d] = %d. Its address: %x\n", i, a[i], &a[i]);
  }
  write(1, "OS by Akshat, Ashish and Vaibhav, version2", 20);
  
  char buf[1024];
  getcwd(buf, 1024);
  write(1, buf, 1024);
  chdir("bin");
  getcwd(buf, 1024);
  return 0;
}
