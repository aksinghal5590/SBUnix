#include <stdlib.h>
#include <libc.h>
#include <unistd.h>
#include <stdio.h>

int a[20];

int main() {
  
  for(int i = 0; i < 20; i++)
      a[i] = 0;
  write(1, "OS by Akshat, Ashish and Vaibhav, version1", 20);
  char buf[10] = "Vaibhav";
  write(1, buf, 10);
  
  //  yield();
//  while(1)
//  {
//    ;
//  }
  return 0;
}
