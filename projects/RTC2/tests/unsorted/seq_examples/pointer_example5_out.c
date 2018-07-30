#include "foo2_c.h" 
bool LessThan_IDK_691_Add(struct VoidStruct ,struct VoidStruct );
void Assign_Overload(struct VoidStruct ,unsigned int *);
struct VoidStruct malloc_overload(unsigned long );

struct VoidStruct 
{
  void *ptr;
  unsigned long long addr;
}
;
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#define PTR_SIZE 100
#define PTR2_SIZE 10
#define PTR3_SIZE 10
#define OUT_OF_BOUNDS_EXCESS 1

int main()
{
  StartClock();
  unsigned int *ptr;
  struct VoidStruct ptr_str;
  ptr_str.VoidStruct::ptr = (reinterpret_cast < void * >  (ptr));
  ptr_str.addr = (reinterpret_cast < unsigned long long  >  ((&ptr)));
  Assign_Overload(ptr_str,((unsigned int *)(malloc_overload((100 * (sizeof(int )))))));
  ptr = ((unsigned int *)ptr_str.VoidStruct::ptr);
  unsigned int *ptr2;
  struct VoidStruct ptr2_str;
  ptr2_str.VoidStruct::ptr = (reinterpret_cast < void * >  (ptr2));
  ptr2_str.addr = (reinterpret_cast < unsigned long long  >  ((&ptr2)));
  Assign_Overload(ptr2_str,((unsigned int *)(malloc_overload((10 * (sizeof(int )))))));
  ptr2 = ((unsigned int *)ptr2_str.VoidStruct::ptr);
  unsigned int *start_ptr;
  struct VoidStruct start_ptr_str;
  start_ptr_str.VoidStruct::ptr = (reinterpret_cast < void * >  (start_ptr));
  start_ptr_str.addr = (reinterpret_cast < unsigned long long  >  ((&start_ptr)));
  Assign_Overload(start_ptr_str,ptr_str);
  start_ptr = ((unsigned int *)start_ptr_str.VoidStruct::ptr);
  unsigned int *start_ptr2;
  struct VoidStruct start_ptr2_str;
  start_ptr2_str.VoidStruct::ptr = (reinterpret_cast < void * >  (start_ptr2));
  start_ptr2_str.addr = (reinterpret_cast < unsigned long long  >  ((&start_ptr2)));
  Assign_Overload(start_ptr2_str,ptr2_str);
  start_ptr2 = ((unsigned int *)start_ptr2_str.VoidStruct::ptr);
  unsigned int *start_ptr3;
  struct VoidStruct start_ptr3_str;
  start_ptr3_str.VoidStruct::ptr = (reinterpret_cast < void * >  (start_ptr3));
  start_ptr3_str.addr = (reinterpret_cast < unsigned long long  >  ((&start_ptr3)));
  Assign_Overload(start_ptr3_str,((unsigned int *)(malloc_overload((100 * (sizeof(unsigned int )))))));
  start_ptr3 = ((unsigned int *)start_ptr3_str.VoidStruct::ptr);
  unsigned int *start_ptr4;
  struct VoidStruct start_ptr4_str;
  start_ptr4_str.VoidStruct::ptr = (reinterpret_cast < void * >  (start_ptr4));
  start_ptr4_str.addr = (reinterpret_cast < unsigned long long  >  ((&start_ptr4)));
  Assign_Overload(start_ptr4_str,start_ptr2_str);
  start_ptr4 = ((unsigned int *)start_ptr4_str.VoidStruct::ptr);
    #if 1
   *start_ptr_str = 1;
   *start_ptr2_str = 1;
   *ptr_str = 3;
   *ptr2_str = 9;
    #endif
  for (unsigned int *new_ptr, struct VoidStruct new_ptr_str, new_ptr_str.VoidStruct::ptr = (reinterpret_cast < void * >  (new_ptr)), new_ptr_str.addr = (reinterpret_cast < unsigned long long  >  ((&new_ptr))), Assign_Overload(new_ptr_str,start_ptr_str), new_ptr = ((unsigned int *)new_ptr_str.VoidStruct::ptr); LessThan_IDK_691_Add(new_ptr_str,(start_ptr_str + 100)); new_ptr_str++) {
     *new_ptr_str = 5;
    printf("%u\n", *new_ptr_str);
  }
  EndClock();
  return 1;
}
