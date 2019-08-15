#include <stdio.h>
#include <stdlib.h>

#define PTR_SIZE 100

int main() {
  int *ptr;
  //~ int *ptr2;

  ptr = (int*)malloc(PTR_SIZE*sizeof(int));

  free(ptr);
  return 0;
}
