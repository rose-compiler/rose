#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#define PTR_SIZE 100
#define PTR2_SIZE 10

int main()
{
  unsigned int *ptr = (unsigned int*)malloc(PTR_SIZE*sizeof(int));
  unsigned int *ptr2 = (unsigned int*)malloc(PTR2_SIZE*sizeof(int));

  unsigned int array1[PTR_SIZE];
  unsigned int array2[PTR2_SIZE];

  unsigned int* start_ptr = ptr;
  unsigned int index;
  for(index = 0; index < PTR_SIZE; index++, start_ptr++) {
    array1[index] = index;
    *start_ptr = index;
  }

  start_ptr = array1;

  for(index = 0; index < PTR_SIZE; index++, start_ptr++)
  {
    printf("array1[index]: %u\n", *start_ptr);
    printf("ptr: %u\n", ptr[index]);
  }

  free(ptr);
  free(ptr2);


  return 0;
}
