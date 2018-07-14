#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#define PTR_SIZE 100
#define PTR2_SIZE 10
#define PTR3_SIZE 10
#define OUT_OF_BOUNDS_EXCESS 1
#define ARRAY_SIZE 29
#define ARRAY2_SIZE 49
#include "foo2_c.h" 

struct node 
{
  struct node *prev;
  struct node *next;
  unsigned int val;
}
;
void print_array(unsigned int *array,unsigned int size);
// FIXME: The parameters are not being replaced with
// structed versions
unsigned int *fn2(unsigned int *,float *,double *,char *);

unsigned int *fn1(unsigned int *input)
{
  unsigned int *nullptr = fn2(input,((float *)input),((double *)input),((char *)input));
  printf("input: %u\n", *input);
  return (unsigned int *)(malloc(400UL));
}

unsigned int *fn2(unsigned int *input,float *input2,double *input3,char *input4)
{
  return 0;
}

int main()
{
  StartClock();
  unsigned int *ptr = (unsigned int *)(malloc(400UL));
  unsigned int *ptr2 = (unsigned int *)(malloc(40UL));
  unsigned int array1[100UL];
  unsigned int array2[10UL];
  unsigned int *start_ptr = ptr;
  unsigned int index;
  for (index = 0U; index < 100U; (index++ , start_ptr++)) {
    array1[index] = index;
     *start_ptr = index;
  }
  struct node stack_node;
  struct node *head = ::new node ;
  head -> node::prev = 0;
  head -> node::next = 0;
  start_ptr = array1;
  unsigned int counter = 0U;
  for (index = 0U; index < 100U; (index++ , start_ptr++)) {
    counter +=  *start_ptr;
    printf("array1[index]: %u\n", *start_ptr);
    printf("ptr: %u\n",ptr[index]);
  }
  printf("%u\n",counter);
  EndClock();
  return 1;
}
