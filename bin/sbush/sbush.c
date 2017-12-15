#include <stdlib.h>
#include <libc.h>
#include <unistd.h>
#include <stdio.h>
#include <dirent.h>

int a[20];

int main() {

 //  for(int i = 0; i < 20; i++) {
	// a[i] = 0;
	// //printf("Value of a[%d] = %d. Its address: %x\n", i, a[i], &a[i]);
 //  }
 //  write(1, "OS by Akshat, Ashish and Vaibhav", 50);
  
 //  char buf[1024];
 //  getcwd(buf, 1024);
 //  write(1, buf, 10);
 //  chdir("bin");
 //  getcwd(buf, 1024);
 //  write(1, buf, 10);
 //  chdir("..");
 //  getcwd(buf, 1024);
 //  write(1, buf, 10);
  
 //  DIR *dirp = opendir("/");
 //  if(dirp != NULL) {
 //    struct dirent *entry = NULL;
 //    while((entry = readdir(dirp)) != NULL) {
 //      write(1, entry->d_name, 256);
 //    }
 //    if(closedir(dirp) == -1)
 //      write(1, "Failed!!", 10);
 //  }

 //  int fd = open("hello.txt", 0);
 //  if(fd < 3)
 //    write(1, "dcvds", 4);
 //  int size;
 //  if((size = read(fd, buf, 10)) > 0)
 //    write(1, buf, size);
 //  if((size = read(fd, buf, 1040)) > 0)
 //    write(1, buf, size);

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
