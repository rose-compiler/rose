#include "foo2_c.h" 
unsigned int *UInt_Deref(struct UIntStruct );
struct UIntStruct UInt_Void_Cast(struct VoidStruct );
struct VoidStruct malloc_wrap(unsigned int size);

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

int main()
{
  StartClock();
  struct UIntStruct ptr_structed0 = UInt_Void_Cast(malloc_wrap(((size_t )(((unsigned long long )100) * (sizeof(int ))))));
  struct UIntStruct ptr2_structed1 = UInt_Void_Cast(malloc_wrap(((size_t )(((unsigned long long )10) * (sizeof(int ))))));
  struct UIntStruct start_ptr_structed2 = ptr_structed0;
  struct UIntStruct start_ptr2_structed3 = ptr2_structed1;
  struct UIntStruct start_ptr3_structed4 = UInt_Void_Cast(malloc_wrap(((size_t )(((unsigned long long )100) * (sizeof(unsigned int ))))));
  struct UIntStruct start_ptr4_structed5 = start_ptr2_structed3;
    #if 1
   *UInt_Deref(start_ptr_structed2) = 1;
   *UInt_Deref(start_ptr2_structed3) = 1;
   *UInt_Deref(ptr_structed0) = 3;
   *UInt_Deref(ptr2_structed1) = 9;
    #endif
  EndClock();
  return 1;
}
