#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#define PTR_SIZE 100
#define PTR2_SIZE 10
#define PTR3_SIZE 10
#define OUT_OF_BOUNDS_EXCESS 1

int main() {

  unsigned int *ptr = (unsigned int*)malloc(PTR_SIZE*sizeof(int));
  unsigned int *ptr2 = (unsigned int*)malloc(PTR2_SIZE*sizeof(int));

  unsigned int* start_ptr = ptr;
  unsigned int* start_ptr2 = ptr2;

  unsigned int *start_ptr3 = (unsigned int*)malloc(PTR_SIZE*sizeof(unsigned int)), *start_ptr4 = start_ptr2;

  *start_ptr = 1;
  *start_ptr2 = 1;

  *ptr = 3;
  *ptr2 = 9;

  free(ptr);
  free(start_ptr2);
  free(start_ptr3);
  return 0;
}
