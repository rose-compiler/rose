#include "foo2_c.h" 
bool UInt_UInt_Struct_LessThan(struct UIntStruct ,struct UIntStruct );
struct UIntStruct UInt_Int_Normal_Add(struct UIntStruct ,int );
struct UIntStruct UInt_Void_Cast(struct VoidStruct );
struct UIntStruct UInt_UInt_Normal_Add(struct UIntStruct ,unsigned int );
unsigned int *UInt_Deref(struct UIntStruct );
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

struct UIntStruct fn1(struct UIntStruct input_structed0)
{
  printf("input: %u\n", *UInt_Deref(input_structed0));
  printf("input: %u\n", *UInt_Deref(input_structed0 = UInt_UInt_Normal_Add(input_structed0,1U)));
  return UInt_Void_Cast(malloc_wrap((100 * (sizeof(unsigned int )))));
}

int main()
{
  StartClock();
  struct UIntStruct ptr_structed1 = UInt_Void_Cast(malloc_wrap(((size_t )(((unsigned long long )100) * (sizeof(int ))))));
  struct UIntStruct ptr2_structed2 = UInt_Void_Cast(malloc_wrap(((size_t )(((unsigned long long )10) * (sizeof(int ))))));
  struct UIntStruct start_ptr_structed3 = ptr_structed1;
  struct UIntStruct start_ptr2_structed4 = ptr2_structed2;
  struct UIntStruct start_ptr3_structed5 = UInt_Void_Cast(malloc_wrap(((size_t )(((unsigned long long )100) * (sizeof(unsigned int ))))));
  struct UIntStruct start_ptr4_structed6 = start_ptr2_structed4;
  struct UIntStruct ptr5_structed7 = ::fn1(start_ptr3_structed5);
  for (struct UIntStruct temp_structed8 = ptr5_structed7; UInt_UInt_Struct_LessThan(temp_structed8,UInt_Int_Normal_Add(ptr5_structed7,100)); temp_structed8 = UInt_UInt_Normal_Add(temp_structed8,1U)) {
    printf("%u\n", *UInt_Deref(temp_structed8));
  }
  EndClock();
  return 1;
}
