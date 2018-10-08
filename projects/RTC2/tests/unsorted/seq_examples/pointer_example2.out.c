#include "foo2_c.h" 
struct IntStruct Int_Void_Cast(struct VoidStruct );
struct VoidStruct malloc_wrap(unsigned int size);

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
  struct IntStruct ptr_structed0;
  struct IntStruct ptr2_structed1;
  ptr_structed0 = Int_Void_Cast(malloc_wrap((100 * (sizeof(int )))));
  ptr2_structed1 = Int_Void_Cast(malloc_wrap((10 * (sizeof(int )))));
  struct IntStruct start_ptr_structed2 = ptr_structed0;
  struct IntStruct start_ptr2_structed3 = ptr2_structed1;
  start_ptr_structed2 = Int_Void_Cast(malloc_wrap((100 * (sizeof(int )))));
  start_ptr2_structed3 = Int_Void_Cast(malloc_wrap((10 * (sizeof(int )))));
    #if 0
    #endif
  EndClock();
  return 1;
}
