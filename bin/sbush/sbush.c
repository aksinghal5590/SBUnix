#include <stdio.h>
#include <libc.h>


int main() {
  write(1, "OS by Akshat, Ashish and Vaibhav, version1", 20);
  yield();
  write(1, "OS by Akshat, Ashish and Vaibhav, version2", 20);
  yield();
  write(1, "OS by Akshat, Ashish and Vaibhav, version3", 20);
/*  int f = fork();
  if(f ==0 ) {
     write(1, "Ferdman Bhadwa h", 20);
  }*/
  //write(1, "OS ki maa ki", 20);
  /*char buf[1024];
  read(0, buf, 10);
  write(1, buf, 10);
  read(0, buf, 5);
  write(1, buf, 5);
  write(1, "OS by Akshat, Ashish and Vaibhav, version3", 20);
  read(0, buf, 20);
  write(1, buf, 20);*/
  return 0;
}
