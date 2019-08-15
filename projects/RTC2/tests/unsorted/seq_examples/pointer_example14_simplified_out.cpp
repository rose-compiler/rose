#include "foo2_c.h" 
struct UnsignedInt_Type1 UnsignedInt_Type1_Decrement_UnsignedInt_Type1(unsigned long long );
bool b_GreaterOrEqual_UnsignedInt_Type1_UnsignedInt_Type1(struct UnsignedInt_Type1 ,struct UnsignedInt_Type1 );
struct UnsignedInt_Type1 UnsignedInt_Type1_Sub_UnsignedInt_Type1_i(struct UnsignedInt_Type1 ,int );
unsigned int *Deref(struct UnsignedInt_Type1 );
struct UnsignedInt_Type1 UnsignedInt_Type1_Increment_UnsignedInt_Type1(unsigned long long );
bool b_LessThan_UnsignedInt_Type1_UnsignedInt_Type1(struct UnsignedInt_Type1 ,struct UnsignedInt_Type1 );
struct UnsignedInt_Type1 UnsignedInt_Type1_Add_UnsignedInt_Type1_i(struct UnsignedInt_Type1 ,int );
struct UnsignedInt_Type1 UnsignedInt_Type1_Assign_UnsignedInt_Type1_UnsignedInt_Type1(unsigned long long ,struct UnsignedInt_Type1 );
struct UnsignedInt_Type1 create_struct(unsigned int *,unsigned long long );
struct UnsignedInt_Type1 Assign(struct UnsignedInt_Type1 *,struct UnsignedInt_Type1 );
struct UnsignedInt_Type1 UnsignedInt_Type1_Cast_Void_Type0(struct Void_Type0 );

struct UnsignedInt_Type1 
{
  unsigned int *ptr;
  unsigned long long addr;
}
;
struct Void_Type0 malloc_overload(int );

struct Void_Type0 
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
#define ARRAY_SIZE 29
#define ARRAY2_SIZE 49

int main()
{
  StartClock();
  unsigned int *no_init_ptr;
  unsigned int *ptr;
  struct UnsignedInt_Type1 ptr_str;
  ((ptr_str.Void_Type0::ptr = ptr , ptr_str.addr = (reinterpret_cast < unsigned long long  >  ((&ptr)))) , ((Assign(&ptr_str,UnsignedInt_Type1_Cast_Void_Type0(malloc_overload(400UL))) , ptr = ptr_str.Void_Type0::ptr)));
  unsigned int *ptr2;
  struct UnsignedInt_Type1 ptr2_str;
  ((ptr2_str.Void_Type0::ptr = ptr2 , ptr2_str.addr = (reinterpret_cast < unsigned long long  >  ((&ptr2)))) , ((Assign(&ptr2_str,UnsignedInt_Type1_Cast_Void_Type0(malloc_overload(40UL))) , ptr2 = ptr2_str.Void_Type0::ptr)));
  unsigned int *ptr_index;
  unsigned int counter = 0U;
  struct UnsignedInt_Type1 UnsignedInt_Type1_ovl_2;
  struct UnsignedInt_Type1 UnsignedInt_Type1_ovl_3;
  for ((((UnsignedInt_Type1_ovl_2 = create_struct(ptr_index,(reinterpret_cast < unsigned long long  >  ((&ptr_index)))) , UnsignedInt_Type1_Assign_UnsignedInt_Type1_UnsignedInt_Type1((reinterpret_cast < unsigned long long  >  ((&UnsignedInt_Type1_ovl_2))),ptr_str)) , ((unsigned int *)( *(reinterpret_cast < void ** >  (UnsignedInt_Type1_ovl_2.addr)))) = UnsignedInt_Type1_ovl_2.Void_Type0::ptr) , UnsignedInt_Type1_ovl_2); b_LessThan_UnsignedInt_Type1_UnsignedInt_Type1(create_struct(ptr_index,(reinterpret_cast < unsigned long long  >  ((&ptr_index)))),UnsignedInt_Type1_Add_UnsignedInt_Type1_i(ptr_str,100)); (((UnsignedInt_Type1_ovl_3 = create_struct(ptr_index,(reinterpret_cast < unsigned long long  >  ((&ptr_index)))) , UnsignedInt_Type1_Increment_UnsignedInt_Type1((reinterpret_cast < unsigned long long  >  ((&UnsignedInt_Type1_ovl_3))))) , ((unsigned int *)( *(reinterpret_cast < void ** >  (UnsignedInt_Type1_ovl_3.addr)))) = UnsignedInt_Type1_ovl_3.Void_Type0::ptr) , UnsignedInt_Type1_ovl_3)) {
     *Deref(create_struct(ptr_index,(reinterpret_cast < unsigned long long  >  ((&ptr_index))))) = counter++;
  }
  struct UnsignedInt_Type1 UnsignedInt_Type1_ovl_4;
  struct UnsignedInt_Type1 UnsignedInt_Type1_ovl_5;
  for ((((UnsignedInt_Type1_ovl_4 = create_struct(ptr_index,(reinterpret_cast < unsigned long long  >  ((&ptr_index)))) , UnsignedInt_Type1_Assign_UnsignedInt_Type1_UnsignedInt_Type1((reinterpret_cast < unsigned long long  >  ((&UnsignedInt_Type1_ovl_4))),UnsignedInt_Type1_Sub_UnsignedInt_Type1_i(UnsignedInt_Type1_Add_UnsignedInt_Type1_i(ptr_str,100),1))) , ((unsigned int *)( *(reinterpret_cast < void ** >  (UnsignedInt_Type1_ovl_4.addr)))) = UnsignedInt_Type1_ovl_4.Void_Type0::ptr) , UnsignedInt_Type1_ovl_4); b_GreaterOrEqual_UnsignedInt_Type1_UnsignedInt_Type1(create_struct(ptr_index,(reinterpret_cast < unsigned long long  >  ((&ptr_index)))),ptr_str); (((UnsignedInt_Type1_ovl_5 = create_struct(ptr_index,(reinterpret_cast < unsigned long long  >  ((&ptr_index)))) , UnsignedInt_Type1_Decrement_UnsignedInt_Type1((reinterpret_cast < unsigned long long  >  ((&UnsignedInt_Type1_ovl_5))))) , ((unsigned int *)( *(reinterpret_cast < void ** >  (UnsignedInt_Type1_ovl_5.addr)))) = UnsignedInt_Type1_ovl_5.Void_Type0::ptr) , UnsignedInt_Type1_ovl_5)) {
    printf("%u\n", *Deref(create_struct(ptr_index,(reinterpret_cast < unsigned long long  >  ((&ptr_index))))));
  }
  EndClock();
  return 1;
}
