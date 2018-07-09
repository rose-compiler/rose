#include "foo2_c.h" 
bool UInt_UInt_Struct_Equality(struct UIntStruct ,struct UIntStruct );
bool UInt_UInt_Struct_NotEqual(struct UIntStruct ,struct UIntStruct );
struct UIntStruct UInt_UInt_Normal_Add(struct UIntStruct ,unsigned int );
bool UInt_UInt_Struct_LessThan(struct UIntStruct ,struct UIntStruct );
struct UIntStruct UInt_Int_Normal_Add(struct UIntStruct ,int );
struct UIntStruct UInt_Int_SpecialCast(int );
struct UIntStruct UInt_Long_SpecialCast(long );
struct UIntStruct UInt_Void_Cast(struct VoidStruct );
unsigned int *UInt_Deref(struct UIntStruct );
struct CharStruct Char_UInt_Cast(struct UIntStruct );
struct DoubleStruct Double_UInt_Cast(struct UIntStruct );
struct FloatStruct Float_UInt_Cast(struct UIntStruct );
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
struct UIntStruct fn2(struct UIntStruct ,struct FloatStruct ,struct DoubleStruct ,struct CharStruct );

struct UIntStruct fn1(struct UIntStruct input_structed0)
{
  struct UIntStruct nullptr_structed1 = ::fn2(input_structed0,Float_UInt_Cast(input_structed0),Double_UInt_Cast(input_structed0),Char_UInt_Cast(input_structed0));
  printf("input: %u\n", *UInt_Deref(input_structed0));
  return UInt_Void_Cast(malloc_wrap((100 * (sizeof(unsigned int )))));
}

struct UIntStruct fn2(struct UIntStruct input_structed2,struct FloatStruct input2_structed3,struct DoubleStruct input3_structed4,struct CharStruct input4_structed5)
{
  return UInt_Int_SpecialCast(0);
}

int main()
{
  StartClock();
  struct UIntStruct ptr_structed6 = UInt_Void_Cast(malloc_wrap(((size_t )(((unsigned long long )100) * (sizeof(int ))))));
  struct UIntStruct ptr2_structed7 = UInt_Void_Cast(malloc_wrap(((size_t )(((unsigned long long )10) * (sizeof(int ))))));
  struct UIntStruct start_ptr_structed8 = ptr_structed6;
  struct UIntStruct start_ptr2_structed9 = ptr2_structed7;
  struct UIntStruct start_ptr3_structed10 = UInt_Void_Cast(malloc_wrap(((size_t )(((unsigned long long )100) * (sizeof(unsigned int ))))));
  struct UIntStruct start_ptr4_structed11 = start_ptr2_structed9;
    #if 1
   *UInt_Deref(start_ptr_structed8) = 1;
   *UInt_Deref(start_ptr2_structed9) = 1;
   *UInt_Deref(ptr_structed6) = 3;
   *UInt_Deref(ptr2_structed7) = 9;
    #endif
  for (struct UIntStruct new_ptr_structed12 = start_ptr_structed8; UInt_UInt_Struct_LessThan(new_ptr_structed12,UInt_Int_Normal_Add(start_ptr_structed8,100)); new_ptr_structed12 = UInt_UInt_Normal_Add(new_ptr_structed12,1U)) {
     *UInt_Deref(new_ptr_structed12) = 5;
    printf("%u\n", *UInt_Deref(new_ptr_structed12));
  }
  struct UIntStruct whileptr_structed13 = UInt_Int_SpecialCast(0);
  do {
    struct UIntStruct doptr_structed14 = start_ptr_structed8;
    for (struct UIntStruct forptr2_structed15 = UInt_Void_Cast(malloc_wrap(((size_t )(((unsigned long long )100) * (sizeof(unsigned int )))))), struct UIntStruct doptr2_structed16 = forptr2_structed15; UInt_UInt_Struct_LessThan(doptr2_structed16,UInt_Int_Normal_Add(forptr2_structed15,100)); doptr2_structed16 = UInt_UInt_Normal_Add(doptr2_structed16,1U)) {
    }
  }while (UInt_UInt_Struct_NotEqual(whileptr_structed13,UInt_Int_SpecialCast(0)));
  struct UIntStruct tempptr_structed17 = start_ptr_structed8;
  if (UInt_UInt_Struct_Equality(whileptr_structed13,UInt_Int_SpecialCast(0))) {
    UInt_Int_Normal_Add(start_ptr_structed8,(100 - 1));
     *UInt_Deref(start_ptr_structed8) = 10;
  }
  start_ptr_structed8 = tempptr_structed17;
  printf("Final print\n");
  for (struct UIntStruct new_ptr_structed18 = start_ptr_structed8; UInt_UInt_Struct_LessThan(new_ptr_structed18,UInt_Int_Normal_Add(start_ptr_structed8,100)); new_ptr_structed18 = UInt_UInt_Normal_Add(new_ptr_structed18,1U)) {
    printf("%u\n", *UInt_Deref(new_ptr_structed18));
  }
  printf("Final print -end\n");
  struct UIntStruct p_structed19 = UInt_Void_Cast(malloc_wrap(((size_t )(((unsigned long long )100) * (sizeof(unsigned int ))))));
  struct UIntStruct q_structed20;
  unsigned int y = (((((q_structed20 = p_structed19) ,  *UInt_Deref(q_structed20) = 5)) ,  *UInt_Deref(q_structed20)));
  struct UIntStruct fnptr_structed21 = ::fn1(start_ptr_structed8);
  struct UIntStruct fnptr2_structed22;
  fnptr2_structed22 = ::fn1(fnptr_structed21);
  struct UIntStruct complexptr_structed23 = UInt_Int_Normal_Add(start_ptr_structed8,1);
  unsigned int complexexp = (( *UInt_Deref(ptr_structed6) +  *UInt_Deref(::fn1(start_ptr_structed8 = UInt_UInt_Normal_Add(start_ptr_structed8,1U)))) -  *UInt_Deref(p_structed19));
// Add more
  EndClock();
  return 1;
}
