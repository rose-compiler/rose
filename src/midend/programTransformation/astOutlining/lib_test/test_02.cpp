#include "outlining_lib.h" 

#include <stdio.h>
#include <assert.h>
int abxyz;

/* original function before outlining
 
int abxyz;
 
void foo(int x)
   {
     abxyz = 42;
  // abxyz = 42;
   }
 
 * */
void foov1(int x)
{  
  if (rose_dynamic_library_exists("./master_shared_library.so")) {
    void (*OUT_1_test_02_cpp_0p)(void **);
    void *__out_argv1__11178__[1];

    __out_argv1__11178__[0] = ((void *)(&abxyz));  // global variable passed into the outlined function

    OUT_1_test_02_cpp_0p = findFunctionUsingDlopen("OUT_1_test_02_cpp_0","./master_shared_library.so");
    ( *OUT_1_test_02_cpp_0p)(__out_argv1__11178__);
 
  }
}

void foov2(int x)
{  
  //findAndCallFunctionUsingDlopen (2,"OUT_1_test_02_cpp_0", (void *)(&abxyz));  
  findAndCallFunctionUsingDlopen (3, "OUT_1_test_02_cpp_0", "./master_shared_library.so", (void *)(&abxyz));  
}


int main (void)
{
  int x=10;
  foov2(x);

  printf("x=%d\n",x);
  assert (x==10);

  printf("abxyz=%d\n",abxyz);
  assert (abxyz==42);

  return 0; 
}
