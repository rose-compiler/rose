#include "foo2_c.h" 

struct LongIntStruct 
{
  long *ptr2obj;
  long *L;
  long *H;
}
;

struct VoidStruct 
{
  void *ptr2obj;
  void *L;
  void *H;
}
;

struct CharStruct 
{
  char *ptr2obj;
  char *L;
  char *H;
}
;

struct DoubleStruct 
{
  double *ptr2obj;
  double *L;
  double *H;
}
;

struct FloatStruct 
{
  float *ptr2obj;
  float *L;
  float *H;
}
;

struct IntStruct 
{
  int *ptr2obj;
  int *L;
  int *H;
}
;

struct UIntStruct 
{
  unsigned int *ptr2obj;
  unsigned int *L;
  unsigned int *H;
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

class User_structed 
{
  public: struct VoidStruct *user_ptr1_structed;
}
;

struct User 
{
  float *user_ptr1;
}
;

class Base_structed 
{
  public: struct VoidStruct *ptr1_structed;
  struct VoidStruct *ptr2_structed;
  struct VoidStruct *ptr3_structed;
  struct VoidStruct *ptr4_structed;
  class User_structed *str_ptr1_structed;
}
;

class Base 
{
  private: unsigned int *ptr1;
  unsigned int *ptr2;
  unsigned int var1;
  char var2;
  float var3;
  float *ptr3;
  unsigned int *ptr4;
  struct User *str_ptr1;
  

  public: virtual inline void print()
{
    struct VoidStruct *print_ptr1_structed;
    int *print_ptr1;
    printf("This is base class\n");
  }
}
;

class Derived1_structed : public Base_structed
{
  public: struct VoidStruct *der1_ptr1_structed;
  struct VoidStruct *der1_ptr2_structed;
}
;

class Derived1 : public Base
{
  private: unsigned int *der1_ptr1;
  float *der1_ptr2;
  

  public: virtual inline void print()
{
    struct VoidStruct *print_ptr1_structed;
    int *print_ptr1;
    printf("This is Derived1 class\n");
  }
}
;

class Derived2_structed : public Derived1_structed
{
  public: struct VoidStruct *der2_ptr1_structed;
  struct VoidStruct *der2_ptr2_structed;
  class Base_structed *der2_base_ptr1_structed;
  class Derived1_structed der2_der1_obj_structed;
}
;

class Derived2 : public Derived1
{
  private: unsigned int *der2_ptr1;
  float *der2_ptr2;
  class Base *der2_base_ptr1;
  class Derived1 der2_der1_obj;
  

  public: virtual inline void print()
{
    struct VoidStruct *print_ptr1_structed;
    int *print_ptr1;
    printf("This is Derived2 class\n");
  }
}
;

class Base *fn1(int *input1,char input4,float *input2,class Derived2 *input3,class Base input5)
{
  return (class Base *)((class Derived1 *)input3);
}

int main()
{
  StartClock();
  struct VoidStruct *ptr_structed;
  int *ptr = (int *)(malloc((100 * (sizeof(int )))));
  struct VoidStruct *ptr2_structed;
  int *ptr2 = (int *)(malloc((10 * (sizeof(int )))));
  class Base_structed base_obj_structed;
  class Base base_obj;
  class Base_structed *base_ptr_structed;
  class Base *base_ptr = new Base ;
  base_ptr ->  print ();
  class Base_structed *base_ptr2_structed;
  class Base *base_ptr2 = base_ptr;
  base_ptr ->  print ();
  class Derived1_structed der1_obj_structed;
  class Derived1 der1_obj;
  base_ptr = (&der1_obj);
  ( *(dynamic_cast < class Derived1 * >  (base_ptr))). print ();
  class Derived2_structed *der2_ptr_structed;
  class Derived2 *der2_ptr = new Derived2 ;
  base_ptr = der2_ptr;
  ( *(dynamic_cast < class Derived2 * >  (base_ptr))). print ();
  der2_ptr = (dynamic_cast < class Derived2 * >  ((fn1(ptr,'a',((float *)ptr2),der2_ptr,(base_obj)))));
  struct VoidStruct *start_ptr_structed;
  int *start_ptr = ptr;
  struct VoidStruct *start_ptr2_structed;
  int *start_ptr2 = ptr2;
// Crossing the boundary of ptr. The condition should
// be less than, not less than or equal to
// ptr[PTR_SIZE] is an out-of-bounds access
  for (int index = 0; index <= (100 + 1); index++) {
     *ptr = index;
    ptr++;
  }
// Resetting ptr to start_ptr, so that it points to the beginning
// of the allocation
  ptr = start_ptr;
// Printing what we wrote above
  for (int index = 0; index <= (100 + 1); index++) {
    printf("ptr[%d]=%d\n",index, *ptr);
    ptr++;
  }
  EndClock();
  return 1;
}

struct VoidPtr_Struct 
{
  int *obj;
  struct VoidStruct *tracker;
}
;

struct VoidPtr_Struct 
{
  float *obj;
  struct VoidStruct *tracker;
}
;

struct VoidPtr_Struct 
{
  class Derived2 *obj;
  class Derived2_structed *tracker;
}
;

struct Base_Struct 
{
  class Base obj;
  class Base_structed tracker;
}
;
