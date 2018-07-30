#include "foo2_c.h" 
struct UnsignedInt_Type0 UnsignedInt_Type0_PlusAssign_UnsignedInt_Type0_i(struct UnsignedInt_Type0 *,int );
bool b_Equality_UnsignedInt_Type0_UnsignedInt_Type0(struct UnsignedInt_Type0 ,struct UnsignedInt_Type0 );
bool b_NotEqual_UnsignedInt_Type0_UnsignedInt_Type0(struct UnsignedInt_Type0 ,struct UnsignedInt_Type0 );
struct UnsignedInt_Type0 UnsignedInt_Type0_Increment_UnsignedInt_Type0(struct UnsignedInt_Type0 *);
bool b_LessThan_UnsignedInt_Type0_UnsignedInt_Type0(struct UnsignedInt_Type0 ,struct UnsignedInt_Type0 );
struct UnsignedInt_Type0 UnsignedInt_Type0_Add_UnsignedInt_Type0_i(struct UnsignedInt_Type0 ,int );
struct UnsignedInt_Type0 UnsignedInt_Type0_Assign_UnsignedInt_Type0_UnsignedInt_Type0(struct UnsignedInt_Type0 *,struct UnsignedInt_Type0 );
int *Deref(struct Int_Type5 );
struct Int_Type5 Int_Type5_Decrement_Int_Type5(struct Int_Type5 *);
struct Int_Type5 Int_Type5_Increment_Int_Type5(struct Int_Type5 *);
struct Int_Type5 Assign(struct Int_Type5 *,struct Int_Type5 );
struct Int_Type5 AddressOf(unsigned long long ,unsigned int );

struct Int_Type5 
{
  int *ptr;
  unsigned long long addr;
}
;
struct UnsignedInt_Type0 UnsignedInt_Type0_Cast_i_ValueCast(int );
struct UnsignedInt_Type0 UnsignedInt_Type0_Cast_Void_Type4(struct Void_Type4 );
struct Void_Type4 malloc_overload(int );

struct Void_Type4 
{
  void *ptr;
  unsigned long long addr;
}
;
unsigned int *Deref(struct UnsignedInt_Type0 );
struct UnsignedInt_Type0 Assign(struct UnsignedInt_Type0 *,struct UnsignedInt_Type0 );
struct Char_Type3 Char_Type3_Cast_UnsignedInt_Type0(struct UnsignedInt_Type0 );
struct Double_Type2 Double_Type2_Cast_UnsignedInt_Type0(struct UnsignedInt_Type0 );
struct Float_Type1 Float_Type1_Cast_UnsignedInt_Type0(struct UnsignedInt_Type0 );
void create_entry(unsigned long long ,unsigned long long );

struct Char_Type3 
{
  char *ptr;
  unsigned long long addr;
}
;

struct Double_Type2 
{
  double *ptr;
  unsigned long long addr;
}
;

struct Float_Type1 
{
  float *ptr;
  unsigned long long addr;
}
;

struct UnsignedInt_Type0 
{
  unsigned int *ptr;
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
#define ARRAY_SIZE 29
#define ARRAY2_SIZE 49
void print_array(struct UnsignedInt_Type0 array_str,unsigned int size);
struct UnsignedInt_Type0 fn2(struct UnsignedInt_Type0 ,struct Float_Type1 ,struct Double_Type2 ,struct Char_Type3 );

struct UnsignedInt_Type0 fn1(struct UnsignedInt_Type0 input_str)
{
  unsigned int *input_recr;
  (create_entry((reinterpret_cast < unsigned long long  >  ((&input_recr))),input_str.addr) , ((input_recr = input_str.ptr , input_str.addr = (reinterpret_cast < unsigned long long  >  ((&input_recr))))));
  unsigned int *nullptr;
  struct UnsignedInt_Type0 nullptr_str;
  ((nullptr_str.ptr = nullptr , nullptr_str.addr = (reinterpret_cast < unsigned long long  >  ((&nullptr)))) , ((Assign(&nullptr_str,::fn2(input_str,Float_Type1_Cast_UnsignedInt_Type0(input_str),Double_Type2_Cast_UnsignedInt_Type0(input_str),Char_Type3_Cast_UnsignedInt_Type0(input_str))) , nullptr = nullptr_str.ptr)));
  printf("input: %u\n", *Deref(input_str));
  return UnsignedInt_Type0_Cast_Void_Type4(malloc_overload(400UL));
}

struct UnsignedInt_Type0 fn2(struct UnsignedInt_Type0 input_str,struct Float_Type1 input2_str,struct Double_Type2 input3_str,struct Char_Type3 input4_str)
{
  char *input4_recr;
  (create_entry((reinterpret_cast < unsigned long long  >  ((&input4_recr))),input4_str.addr) , ((input4_recr = input4_str.ptr , input4_str.addr = (reinterpret_cast < unsigned long long  >  ((&input4_recr))))));
  double *input3_recr;
  (create_entry((reinterpret_cast < unsigned long long  >  ((&input3_recr))),input3_str.addr) , ((input3_recr = input3_str.ptr , input3_str.addr = (reinterpret_cast < unsigned long long  >  ((&input3_recr))))));
  float *input2_recr;
  (create_entry((reinterpret_cast < unsigned long long  >  ((&input2_recr))),input2_str.addr) , ((input2_recr = input2_str.ptr , input2_str.addr = (reinterpret_cast < unsigned long long  >  ((&input2_recr))))));
  unsigned int *input_recr;
  (create_entry((reinterpret_cast < unsigned long long  >  ((&input_recr))),input_str.addr) , ((input_recr = input_str.ptr , input_str.addr = (reinterpret_cast < unsigned long long  >  ((&input_recr))))));
  return UnsignedInt_Type0_Cast_i_ValueCast(0);
}

int main()
{
  StartClock();
  printf("main - begin\n");
  unsigned int *ptr;
  struct UnsignedInt_Type0 ptr_str;
  ((ptr_str.UnsignedInt_Type0::ptr = ptr , ptr_str.addr = (reinterpret_cast < unsigned long long  >  ((&ptr)))) , ((Assign(&ptr_str,UnsignedInt_Type0_Cast_Void_Type4(malloc_overload(400UL))) , ptr = ptr_str.UnsignedInt_Type0::ptr)));
  unsigned int *ptr2;
  struct UnsignedInt_Type0 ptr2_str;
  ((ptr2_str.UnsignedInt_Type0::ptr = ptr2 , ptr2_str.addr = (reinterpret_cast < unsigned long long  >  ((&ptr2)))) , ((Assign(&ptr2_str,UnsignedInt_Type0_Cast_Void_Type4(malloc_overload(40UL))) , ptr2 = ptr2_str.UnsignedInt_Type0::ptr)));
// Added code
  int stack_var = 5;
  int *stack_ptr;
  struct Int_Type5 stack_ptr_str;
  ((stack_ptr_str.UnsignedInt_Type0::ptr = stack_ptr , stack_ptr_str.addr = (reinterpret_cast < unsigned long long  >  ((&stack_ptr)))) , ((Assign(&stack_ptr_str,AddressOf((reinterpret_cast < unsigned long long  >  ((&stack_var))),sizeof(int ))) , stack_ptr = stack_ptr_str.UnsignedInt_Type0::ptr)));
  ((Int_Type5_Increment_Int_Type5(&stack_ptr_str) , stack_ptr = stack_ptr_str.UnsignedInt_Type0::ptr) , stack_ptr_str);
  ((Int_Type5_Decrement_Int_Type5(&stack_ptr_str) , stack_ptr = stack_ptr_str.UnsignedInt_Type0::ptr) , stack_ptr_str);
  ( *Deref(stack_ptr_str))--;
  printf("stack_var: %u, stack_ptr: %u\n",stack_var, *Deref(stack_ptr_str));
// end added code
  printf("ptr & ptr2 - done\n");
  unsigned int *start_ptr;
  struct UnsignedInt_Type0 start_ptr_str;
  ((start_ptr_str.UnsignedInt_Type0::ptr = start_ptr , start_ptr_str.addr = (reinterpret_cast < unsigned long long  >  ((&start_ptr)))) , ((Assign(&start_ptr_str,ptr_str) , start_ptr = start_ptr_str.UnsignedInt_Type0::ptr)));
  unsigned int *start_ptr2;
  struct UnsignedInt_Type0 start_ptr2_str;
  ((start_ptr2_str.UnsignedInt_Type0::ptr = start_ptr2 , start_ptr2_str.addr = (reinterpret_cast < unsigned long long  >  ((&start_ptr2)))) , ((Assign(&start_ptr2_str,ptr2_str) , start_ptr2 = start_ptr2_str.UnsignedInt_Type0::ptr)));
  printf("start_ptr & start_ptr2 - done\n");
  unsigned int *start_ptr3;
  struct UnsignedInt_Type0 start_ptr3_str;
  ((start_ptr3_str.UnsignedInt_Type0::ptr = start_ptr3 , start_ptr3_str.addr = (reinterpret_cast < unsigned long long  >  ((&start_ptr3)))) , ((Assign(&start_ptr3_str,UnsignedInt_Type0_Cast_Void_Type4(malloc_overload(400UL))) , start_ptr3 = start_ptr3_str.UnsignedInt_Type0::ptr)));
  unsigned int *start_ptr4;
  struct UnsignedInt_Type0 start_ptr4_str;
  ((start_ptr4_str.UnsignedInt_Type0::ptr = start_ptr4 , start_ptr4_str.addr = (reinterpret_cast < unsigned long long  >  ((&start_ptr4)))) , ((Assign(&start_ptr4_str,start_ptr2_str) , start_ptr4 = start_ptr4_str.UnsignedInt_Type0::ptr)));
  printf("start_ptr3 - done\n");
    #if 1
   *Deref(start_ptr_str) = 1U;
   *Deref(start_ptr2_str) = 1U;
  printf("start_ptr: %u\n", *Deref(start_ptr_str));
  printf("start_ptr2: %u\n", *Deref(start_ptr2_str));
   *Deref(ptr_str) = 3U;
   *Deref(ptr2_str) = 9U;
  printf("start_ptr: %u\n", *Deref(start_ptr_str));
  printf("start_ptr2: %u\n", *Deref(start_ptr2_str));
    #endif
  printf("start_ptr, start_ptr2, ptr, ptr2 - done\n");
  unsigned int *new_ptr;
  struct UnsignedInt_Type0 new_ptr_str;
  (new_ptr_str.UnsignedInt_Type0::ptr = new_ptr , new_ptr_str.addr = (reinterpret_cast < unsigned long long  >  ((&new_ptr))));
  for (((UnsignedInt_Type0_Assign_UnsignedInt_Type0_UnsignedInt_Type0(&new_ptr_str,start_ptr_str) , new_ptr = new_ptr_str.UnsignedInt_Type0::ptr) , new_ptr_str); b_LessThan_UnsignedInt_Type0_UnsignedInt_Type0(new_ptr_str,UnsignedInt_Type0_Add_UnsignedInt_Type0_i(start_ptr_str,100)); ((UnsignedInt_Type0_Increment_UnsignedInt_Type0(&new_ptr_str) , new_ptr = new_ptr_str.UnsignedInt_Type0::ptr) , new_ptr_str)) {
     *Deref(new_ptr_str) = 5U;
    printf("%u\n", *Deref(new_ptr_str));
  }
  for (((UnsignedInt_Type0_Assign_UnsignedInt_Type0_UnsignedInt_Type0(&new_ptr_str,start_ptr2_str) , new_ptr = new_ptr_str.UnsignedInt_Type0::ptr) , new_ptr_str); b_LessThan_UnsignedInt_Type0_UnsignedInt_Type0(new_ptr_str,UnsignedInt_Type0_Add_UnsignedInt_Type0_i(start_ptr2_str,10)); ((UnsignedInt_Type0_Increment_UnsignedInt_Type0(&new_ptr_str) , new_ptr = new_ptr_str.UnsignedInt_Type0::ptr) , new_ptr_str)) {
     *Deref(new_ptr_str) = 25U;
    printf("%u\n", *Deref(new_ptr_str));
  }
  printf("for loop 1 - done\n");
  unsigned int *whileptr;
  struct UnsignedInt_Type0 whileptr_str;
  ((whileptr_str.UnsignedInt_Type0::ptr = whileptr , whileptr_str.addr = (reinterpret_cast < unsigned long long  >  ((&whileptr)))) , ((Assign(&whileptr_str,UnsignedInt_Type0_Cast_i_ValueCast(0)) , whileptr = whileptr_str.UnsignedInt_Type0::ptr)));
  do {
    unsigned int *doptr;
    struct UnsignedInt_Type0 doptr_str;
    ((doptr_str.UnsignedInt_Type0::ptr = doptr , doptr_str.addr = (reinterpret_cast < unsigned long long  >  ((&doptr)))) , ((Assign(&doptr_str,start_ptr_str) , doptr = doptr_str.UnsignedInt_Type0::ptr)));
    unsigned int *forptr2;
    struct UnsignedInt_Type0 forptr2_str;
    ((forptr2_str.UnsignedInt_Type0::ptr = forptr2 , forptr2_str.addr = (reinterpret_cast < unsigned long long  >  ((&forptr2)))) , ((Assign(&forptr2_str,UnsignedInt_Type0_Cast_Void_Type4(malloc_overload(400UL))) , forptr2 = forptr2_str.UnsignedInt_Type0::ptr)));
    unsigned int *doptr2;
    struct UnsignedInt_Type0 doptr2_str;
    ((doptr2_str.UnsignedInt_Type0::ptr = doptr2 , doptr2_str.addr = (reinterpret_cast < unsigned long long  >  ((&doptr2)))) , ((Assign(&doptr2_str,forptr2_str) , doptr2 = doptr2_str.UnsignedInt_Type0::ptr)));
    for (; b_LessThan_UnsignedInt_Type0_UnsignedInt_Type0(doptr2_str,UnsignedInt_Type0_Add_UnsignedInt_Type0_i(forptr2_str,100)); ((UnsignedInt_Type0_Increment_UnsignedInt_Type0(&doptr2_str) , doptr2 = doptr2_str.UnsignedInt_Type0::ptr) , doptr2_str)) {
    }
  }while (b_NotEqual_UnsignedInt_Type0_UnsignedInt_Type0(whileptr_str,UnsignedInt_Type0_Cast_i_ValueCast(0)));
  unsigned int *tempptr;
  struct UnsignedInt_Type0 tempptr_str;
  ((tempptr_str.UnsignedInt_Type0::ptr = tempptr , tempptr_str.addr = (reinterpret_cast < unsigned long long  >  ((&tempptr)))) , ((Assign(&tempptr_str,start_ptr_str) , tempptr = tempptr_str.UnsignedInt_Type0::ptr)));
  if (b_Equality_UnsignedInt_Type0_UnsignedInt_Type0(whileptr_str,UnsignedInt_Type0_Cast_i_ValueCast(0))) {
    ((UnsignedInt_Type0_PlusAssign_UnsignedInt_Type0_i(&start_ptr_str,99) , start_ptr = start_ptr_str.UnsignedInt_Type0::ptr) , start_ptr_str);
     *Deref(start_ptr_str) = 10U;
  }
  ((UnsignedInt_Type0_Assign_UnsignedInt_Type0_UnsignedInt_Type0(&start_ptr_str,tempptr_str) , start_ptr = start_ptr_str.UnsignedInt_Type0::ptr) , start_ptr_str);
  printf("Final print\n");
  for (((UnsignedInt_Type0_Assign_UnsignedInt_Type0_UnsignedInt_Type0(&new_ptr_str,start_ptr_str) , new_ptr = new_ptr_str.UnsignedInt_Type0::ptr) , new_ptr_str); b_LessThan_UnsignedInt_Type0_UnsignedInt_Type0(new_ptr_str,UnsignedInt_Type0_Add_UnsignedInt_Type0_i(start_ptr_str,100)); ((UnsignedInt_Type0_Increment_UnsignedInt_Type0(&new_ptr_str) , new_ptr = new_ptr_str.UnsignedInt_Type0::ptr) , new_ptr_str)) {
    printf("%u\n", *Deref(new_ptr_str));
  }
  printf("Final print -end\n");
// CIRM Review Code
  unsigned int *p;
  struct UnsignedInt_Type0 p_str;
  ((p_str.UnsignedInt_Type0::ptr = p , p_str.addr = (reinterpret_cast < unsigned long long  >  ((&p)))) , ((Assign(&p_str,UnsignedInt_Type0_Cast_Void_Type4(malloc_overload(400UL))) , p = p_str.UnsignedInt_Type0::ptr)));
  unsigned int *index;
  struct UnsignedInt_Type0 index_str;
  ((index_str.UnsignedInt_Type0::ptr = index , index_str.addr = (reinterpret_cast < unsigned long long  >  ((&index)))) , ((Assign(&index_str,p_str) , index = index_str.UnsignedInt_Type0::ptr)));
  for (; b_LessThan_UnsignedInt_Type0_UnsignedInt_Type0(index_str,UnsignedInt_Type0_Add_UnsignedInt_Type0_i(p_str,100)); ((UnsignedInt_Type0_Increment_UnsignedInt_Type0(&index_str) , index = index_str.UnsignedInt_Type0::ptr) , index_str)) {
     *Deref(index_str) = 15U;
    printf("%u\n", *Deref(index_str));
  }
  unsigned int *q;
  struct UnsignedInt_Type0 q_str;
  (q_str.UnsignedInt_Type0::ptr = q , q_str.addr = (reinterpret_cast < unsigned long long  >  ((&q))));
  unsigned int y = ((((((UnsignedInt_Type0_Assign_UnsignedInt_Type0_UnsignedInt_Type0(&q_str,p_str) , q = q_str.UnsignedInt_Type0::ptr) , q_str) ,  *Deref(q_str) = 5U)) ,  *Deref(q_str)));
  printf("q: %u\n", *Deref(q_str));
  printf("p: %u\n", *Deref(p_str));
  printf("y: %u\n",y);
// ------
// Passing pointers to function
  unsigned int *fnptr;
  struct UnsignedInt_Type0 fnptr_str;
  ((fnptr_str.UnsignedInt_Type0::ptr = fnptr , fnptr_str.addr = (reinterpret_cast < unsigned long long  >  ((&fnptr)))) , ((Assign(&fnptr_str,::fn1(start_ptr_str)) , fnptr = fnptr_str.UnsignedInt_Type0::ptr)));
  unsigned int *fnptr2;
  struct UnsignedInt_Type0 fnptr2_str;
  (fnptr2_str.UnsignedInt_Type0::ptr = fnptr2 , fnptr2_str.addr = (reinterpret_cast < unsigned long long  >  ((&fnptr2))));
  ((UnsignedInt_Type0_Assign_UnsignedInt_Type0_UnsignedInt_Type0(&fnptr2_str,::fn1(fnptr_str)) , fnptr2 = fnptr2_str.UnsignedInt_Type0::ptr) , fnptr2_str);
// ------
// Complex expressions
  unsigned int *complexptr;
  struct UnsignedInt_Type0 complexptr_str;
  ((complexptr_str.UnsignedInt_Type0::ptr = complexptr , complexptr_str.addr = (reinterpret_cast < unsigned long long  >  ((&complexptr)))) , ((Assign(&complexptr_str,UnsignedInt_Type0_Add_UnsignedInt_Type0_i(start_ptr_str,1)) , complexptr = complexptr_str.UnsignedInt_Type0::ptr)));
  unsigned int complexexp = (( *Deref(ptr_str) +  *Deref(::fn1((((UnsignedInt_Type0_Increment_UnsignedInt_Type0(&start_ptr_str) , start_ptr = start_ptr_str.UnsignedInt_Type0::ptr) , start_ptr_str))))) -  *Deref(p_str));
  printf("complexptr: %u\n", *Deref(complexptr_str));
  printf("complexexp: %u\n",complexexp);
// Add more
  print_array(p_str,100U);
  print_array(ptr_str,100U);
  print_array(ptr2_str,10U);
  EndClock();
  return 1;
}
