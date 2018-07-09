#include "foo2_c.h" 
void free_wrap(struct VoidStruct );
struct VoidStruct Void_Int_Cast(struct IntStruct );
void fflush_wrap(FILE *);
struct IntStruct Int_UInt_Normal_Add(struct IntStruct ,unsigned int );
int *Int_Deref(struct IntStruct );
bool Int_Int_Struct_NotEqual(struct IntStruct ,struct IntStruct );
struct IntStruct Int_Int_SpecialCast(int );
struct IntStruct Int_Long_SpecialCast(long );
struct IntStruct Int_Void_Cast(struct VoidStruct );
struct VoidStruct malloc_wrap(unsigned long );
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
  struct IntStruct ptr_structed0 = Int_Void_Cast(malloc_wrap(((size_t )(((unsigned long long )100) * (sizeof(int ))))));
  struct IntStruct ptr2_structed1 = Int_Void_Cast(malloc_wrap(((size_t )(((unsigned long long )10) * (sizeof(int ))))));
  (__builtin_expect((!Int_Int_Struct_NotEqual(ptr2_structed1,Int_Int_SpecialCast(0))),0))?__assert_rtn(__func__,"/Users/vanka1/research/compilers/rose_public/rose_build/projects/RTC/pointer_example.cpp",17,"ptr2 != NULL") : ((void )0);
  struct IntStruct start_ptr_structed2 = ptr_structed0;
  struct IntStruct start_ptr2_structed3 = ptr2_structed1;
// Crossing the boundary of ptr. The condition should
// be less than, not less than or equal to
// ptr[PTR_SIZE] is an out-of-bounds access
  for (int index = 0; index <= (100 + 1); index++) {
     *Int_Deref(ptr_structed0) = index;
    ptr_structed0 = Int_UInt_Normal_Add(ptr_structed0,1U);
  }
// Resetting ptr to start_ptr, so that it points to the beginning
// of the allocation
  ptr_structed0 = start_ptr_structed2;
// Printing what we wrote above
  for (int index = 0; index <= (100 + 1); index++) {
    printf("ptr[%d]=%d\n",index, *Int_Deref(ptr_structed0));
    ptr_structed0 = Int_UInt_Normal_Add(ptr_structed0,1U);
  }
#if 0
// Resetting ptr to start_ptr, so that it points to the beginning
// of the allocation
// Memsetting ptr and ptr2 allocations, in one go.
// This is also crossing the boundaries of ptr. It assumes that
// ptr and ptr2 are in contiguous locations
// Resetting ptr to start_ptr, so that it points to the beginning
// of the allocation
// Printing ptr and ptr2 *and* one more beyond ptr2, all using 
// ptr! This still works since malloc asks for more than it needs
// always.
#endif
  (__builtin_expect((!Int_Int_Struct_NotEqual(ptr2_structed1,Int_Int_SpecialCast(0))),0))?__assert_rtn(__func__,"/Users/vanka1/research/compilers/rose_public/rose_build/projects/RTC/pointer_example.cpp",68,"ptr2 != NULL") : ((void )0);
  printf("Before free ptr2\n");
  fflush_wrap(0);
  free_wrap(Void_Int_Cast(ptr2_structed1));
#if 0
    #if 0
// Retrying the print above, after freeing ptr2. This should
// crash--- and it does!
    #endif
// Allocating another pointer
// This allocation might take the place of ptr2. In this case,
// printing ptr beyond its boundaries should be okay
// Nope this also crashes!
#endif    
  EndClock();
  return 1;
}
