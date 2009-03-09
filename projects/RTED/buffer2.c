

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
}
