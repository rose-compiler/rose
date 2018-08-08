#if 1
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
class node ;
class node ;

class node 
{
  public: class node *prev;
  class node *next;
  unsigned int val;
//unsigned int array1[PTR2_SIZE];
  

  inline void set_val(unsigned int input)
{
    (this) -> val = input;
  }
  

  inline unsigned int get_val()
{
    return (this) -> val;
  }
}
;
#endif
//#include "class_lib.h"

int main()
{
  unsigned int *no_init_ptr;
  printf("before ptr\n");
  unsigned int *ptr = (unsigned int *)(malloc((100 * (sizeof(int )))));
  printf("before ptr2\n");
  unsigned int *ptr2 = (unsigned int *)(malloc((10 * (sizeof(int )))));
  printf("before ptr_index\n");
  unsigned int *ptr_index = ptr;
  unsigned int counter = 0;
  printf("before loop1\n");
  for (; ptr_index < (ptr + 100); ptr_index++) {
     *ptr_index = counter++;
    printf("%lu\n",(reinterpret_cast < unsigned long  >  (ptr_index)));
  }
    #if 1
  int array[100UL];
  printf("before loop2\n");
  for (counter = 0; counter < 100; counter++) {
    array[counter] = counter;
  }
  printf("%u\n", *array);
  printf("%u\n",array[0]);
  printf("%u\n", *array);
  printf("Done\n");
    #endif
  printf("before loop3\n");
  for (ptr_index = ((ptr + 100) - 1); ptr_index >= ptr; ptr_index--) {
    printf("%lu\n",(reinterpret_cast < unsigned long  >  (ptr)));
    printf("%lu\n",(reinterpret_cast < unsigned long  >  (ptr_index)));
    printf("%u\n", *ptr_index);
  }
    #if 1
  printf("before loop4\n");
  class node class_array[100UL];
  for (counter = 0; counter < 100; counter++) {
    class_array[counter]. set_val (counter);
  }
  printf("Printing class array\n");
  printf("before loop5\n");
  for (counter = 0; counter < 100; counter++) {
    printf("%u\n",class_array[counter]. get_val ());
  }
    #endif
    #if 1
  int *array_ptr[100UL];
  printf("before loop6\n");
  for (counter = 0; counter < 100; counter++) {
    array_ptr[counter] = ((int *)(malloc((sizeof(int )))));
     *array_ptr[counter] = counter;
  }
  printf("Printing array\n");
  printf("before loop7\n");
  for (counter = 0; counter < 100; counter++) {
    printf("%u\n", *array_ptr[counter]);
  }
    #endif
    #if 1
  class node *class_array_ptr[100UL];
  printf("before loop8\n");
  for (counter = 0; counter < 100; counter++) {
    class_array_ptr[counter] = ((class node *)(malloc((sizeof(class node )))));
    ( *class_array_ptr[counter]). set_val (counter);
  }
  printf("Printing class array\n");
  printf("before loop9\n");
  for (counter = 0; counter < 100; counter++) {
    printf("%u\n",( *class_array_ptr[counter]). get_val ());
  }
    #endif
  return 1;
}
