#include <stdio.h>
#include <libc.h>

int main() {
  write(1, "OS by Akshat, Ashish and Vaibhav, version1", 20);
  yield();
  write(1, "OS by Akshat, Ashish and Vaibhav, version2", 20);
  yield();
  write(1, "OS by Akshat, Ashish and Vaibhav, version3", 20);
  int f = fork();
  // yield();
  if(f > 0 ) {
     write(1, "Ferdman Bhadwa h", 20);
     yield();
     // yield();
  } else if(f ==0 ) {
     write(1, "OS ki maa ki", 20);
     // if (fork() == 0) {
     //    write(1, "OS ki maa ki1", 20);
     // } else {
     //    write(1, "Ferdman Bhadwa h1", 20);
     //    yield();
     // }
  }
 
  /*char buf[1024];
  read(0, buf, 10);
  write(1, buf, 10);
  read(0, buf, 5);
  write(1, buf, 5);
  write(1, "OS by Akshat, Ashish and Vaibhav, version3", 20);
  read(0, buf, 20);
  write(1, buf, 20);*/
  while(1);
  return 0;
}
