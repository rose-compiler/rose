#include "foo2_c.h" 
void Assign_Overload(struct VoidStruct ,struct VoidStruct );
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
  int *ptr;
  struct VoidStruct ptr_str;
  ptr_str.VoidStruct::ptr = (reinterpret_cast < void * >  (ptr));
  ptr_str.addr = (reinterpret_cast < unsigned long long  >  ((&ptr)));
  int *ptr2;
  struct VoidStruct ptr2_str;
  ptr2_str.VoidStruct::ptr = (reinterpret_cast < void * >  (ptr2));
  ptr2_str.addr = (reinterpret_cast < unsigned long long  >  ((&ptr2)));
  ptr_str = ((int *)(malloc_overload((100 * (sizeof(int ))))));
  ptr2_str = ((int *)(malloc_overload((10 * (sizeof(int ))))));
  int *start_ptr;
  struct VoidStruct start_ptr_str;
  start_ptr_str.VoidStruct::ptr = (reinterpret_cast < void * >  (start_ptr));
  start_ptr_str.addr = (reinterpret_cast < unsigned long long  >  ((&start_ptr)));
  Assign_Overload(start_ptr_str,ptr_str);
  start_ptr = ((int *)start_ptr_str.VoidStruct::ptr);
  int *start_ptr2;
  struct VoidStruct start_ptr2_str;
  start_ptr2_str.VoidStruct::ptr = (reinterpret_cast < void * >  (start_ptr2));
  start_ptr2_str.addr = (reinterpret_cast < unsigned long long  >  ((&start_ptr2)));
  Assign_Overload(start_ptr2_str,ptr2_str);
  start_ptr2 = ((int *)start_ptr2_str.VoidStruct::ptr);
  start_ptr_str = ((int *)(malloc_overload((100 * (sizeof(int ))))));
  start_ptr2_str = ((int *)(malloc_overload((10 * (sizeof(int ))))));
    #if 0
    #endif
  EndClock();
  return 1;
}
