#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#define PTR_SIZE 100
#define PTR2_SIZE 10


struct node {
  struct node* prev;
  struct node* next;
  unsigned int val;
  //unsigned int array1[PTR2_SIZE];
};

int main()
{
  unsigned int* ptr = (unsigned int*)malloc(PTR_SIZE*sizeof(int));
  unsigned int* ptr2 = (unsigned int*)malloc(PTR2_SIZE*sizeof(int));

  unsigned int* ptr_index;
  unsigned int  counter = 0;

  for (ptr_index = ptr; ptr_index < ptr + PTR_SIZE; ptr_index++) {
    *ptr_index = counter++;
  }

  int array[10];
  for(counter = 0; counter < PTR_SIZE; counter++) {
    array[counter] = counter; // <<< out of bounds access, 10 < PTR_SIZE
  }

  printf("%u\n", *array);
  printf("%u\n", array[0]);
  printf("%u\n", *array);
  printf("Done\n");

  for(ptr_index = ptr + PTR_SIZE - 1; ptr_index >= ptr; ptr_index--) {
    printf("%u\n", *ptr_index);
  }

  return 0;
}
