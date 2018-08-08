#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#define PTR_SIZE 100
#define PTR2_SIZE 10
#define PTR3_SIZE 10
#define OUT_OF_BOUNDS_EXCESS 1

int main() {
  int *ptr;
  int *ptr2;

  ptr = (int*)malloc(PTR_SIZE*sizeof(int));
  ptr2 = (int*)malloc(PTR2_SIZE*sizeof(int));

  int* start_ptr = ptr;
  int* start_ptr2 = ptr2;

  start_ptr = (int*)malloc(PTR_SIZE*sizeof(int));
  start_ptr2 = (int*)malloc(PTR2_SIZE*sizeof(int));

  *start_ptr = 1;
  *start_ptr2 = 1;

  *ptr = 3;
  *ptr2 = 9;

  free(ptr);
  free(ptr2);
  free(start_ptr);
  free(start_ptr2);
  return 0;
}
