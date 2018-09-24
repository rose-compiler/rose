#include "foo2_c.h" 
bool UInt_UInt_Struct_Equality(struct UIntStruct ,struct UIntStruct );
bool UInt_UInt_Struct_NotEqual(struct UIntStruct ,struct UIntStruct );
struct UIntStruct UInt_Int_SpecialCast(int );
struct UIntStruct UInt_Long_SpecialCast(long );
struct UIntStruct UInt_UInt_Normal_Add(struct UIntStruct ,unsigned int );
bool UInt_UInt_Struct_LessThan(struct UIntStruct ,struct UIntStruct );
struct UIntStruct UInt_Int_Normal_Add(struct UIntStruct ,int );
unsigned int *UInt_Deref(struct UIntStruct );
struct UIntStruct UInt_Void_Cast(struct VoidStruct );
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
  for (struct UIntStruct new_ptr_structed6 = start_ptr_structed2; UInt_UInt_Struct_LessThan(new_ptr_structed6,UInt_Int_Normal_Add(start_ptr_structed2,100)); new_ptr_structed6 = UInt_UInt_Normal_Add(new_ptr_structed6,1U)) {
     *UInt_Deref(new_ptr_structed6) = 5;
    printf("%u\n", *UInt_Deref(new_ptr_structed6));
  }
  struct UIntStruct whileptr_structed7 = UInt_Int_SpecialCast(0);
  do {
    struct UIntStruct doptr_structed8 = start_ptr_structed2;
    for (struct UIntStruct forptr2_structed9 = UInt_Void_Cast(malloc_wrap(((size_t )(((unsigned long long )100) * (sizeof(unsigned int )))))), struct UIntStruct doptr2_structed10 = forptr2_structed9; UInt_UInt_Struct_LessThan(doptr2_structed10,UInt_Int_Normal_Add(forptr2_structed9,100)); doptr2_structed10 = UInt_UInt_Normal_Add(doptr2_structed10,1U)) {
    }
  }while (UInt_UInt_Struct_NotEqual(whileptr_structed7,UInt_Int_SpecialCast(0)));
  struct UIntStruct tempptr_structed11 = start_ptr_structed2;
  if (UInt_UInt_Struct_Equality(whileptr_structed7,UInt_Int_SpecialCast(0))) {
    UInt_Int_Normal_Add(start_ptr_structed2,(100 - 1));
     *UInt_Deref(start_ptr_structed2) = 10;
  }
  start_ptr_structed2 = tempptr_structed11;
  printf("Final print\n");
  for (struct UIntStruct new_ptr_structed12 = start_ptr_structed2; UInt_UInt_Struct_LessThan(new_ptr_structed12,UInt_Int_Normal_Add(start_ptr_structed2,100)); new_ptr_structed12 = UInt_UInt_Normal_Add(new_ptr_structed12,1U)) {
    printf("%u\n", *UInt_Deref(new_ptr_structed12));
  }
  printf("Final print -end\n");
  EndClock();
  return 1;
}
