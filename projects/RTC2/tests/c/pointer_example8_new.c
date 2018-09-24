#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#define PTR_SIZE 100
#define PTR2_SIZE 10

void fill(int* x, int sz, int v)
{
  if (sz < 0) return;

  *x = v;
  fill(x, sz-1, v);
}

unsigned int* fn1(unsigned int* input)
{
  printf("input: %u\n", *input);
  return (unsigned int*)malloc(PTR_SIZE*sizeof(unsigned int));
}


int main()
{

  unsigned int *ptr  = (unsigned int*)malloc(PTR_SIZE*sizeof(int));
  unsigned int *ptr2 = (unsigned int*)malloc(PTR2_SIZE*sizeof(int));

  fill(ptr,  PTR_SIZE,   0);
  fill(ptr2, PTR2_SIZE, -1);

  unsigned int* start_ptr = fn1(ptr);
  unsigned int* start_ptr2 = ptr2;

  unsigned int *start_ptr3 = (unsigned int*)malloc(PTR_SIZE*sizeof(unsigned int)), *start_ptr4 = start_ptr2;

  free(ptr);
  free(ptr2);
  free(start_ptr);
  free(start_ptr3);
  return 0;
}
