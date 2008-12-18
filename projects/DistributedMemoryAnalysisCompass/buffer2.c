#include <unistd.h>
#include <sys/stat.h>
#include <fcntl.h>


#include <stdio.h>
//#include <malloc.h>
#include <stdlib.h>

int main(int argc, char* argv) {
  int* arr = malloc( sizeof(int)*10);

  int i=0;
  for (i=0; i<10;++i) {
    arr[i]=5;
  }
  int x = arr[12];


  int f = open("/home/panas2/dbs/intExample.test",O_WRONLY|O_CREAT);
  write (f, "foo", 3);
  close(f);

}
