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
  //write(1, "OS by Akshat, Ashish and Vaibhav, version1", 20);
  //yield();
  write(1, "OS by Akshat, Ashish and Vaibhav, version2", 20);
  yield();
  //write(1, "OS by Akshat, Ashish and Vaibhav, version3", 20);
  //int f = fork();
  //if(f ==0 ) {
  //   write(1, "Ferdman Bhadwa h", 20);
  //}
  //write(1, "OS ki maa ki", 20);
  
  char buf[1024];
  getcwd(buf, 1024);
  /*int i = 0;
  while(buf[i++] != '\0')
    buf[i] = '\n';
    buf[i+1] = '\0';*/
  write(1, buf, 1024);
  chdir("bin");
  getcwd(buf, 1024);
  /*i = 0;
  while(buf[i++] != '\0')
    buf[i] = '\n';
    buf[i+1] = '\0';*/
  write(1, buf, 1024);
  /*read(0, buf, 10);
  write(1, buf, 10);
  read(0, buf, 5);
  write(1, buf, 5);
  write(1, "OS by Akshat, Ashish and Vaibhav, version3", 20);
  read(0, buf, 20);
  write(1, buf, 20);*/
  return 0;
}
