#include "foo2_c.h" 
struct UnsignedInt_Type1 UnsignedInt_Type1_Decrement_UnsignedInt_Type1(unsigned long long );
bool b_GreaterOrEqual_UnsignedInt_Type1_UnsignedInt_Type1(struct UnsignedInt_Type1 ,struct UnsignedInt_Type1 );
struct UnsignedInt_Type1 UnsignedInt_Type1_Sub_UnsignedInt_Type1_i(struct UnsignedInt_Type1 ,int );
int *__Pb__i__Pe___PntrArrRef_Int_Type5_i(struct Int_Type5 ,int );
int *Deref(struct Int_Type5 );
int *__Pb__i__Pe___PntrArrRef_Int_Type5_Ui(struct Int_Type5 ,unsigned int );
struct Int_Type5 create_struct(int *,unsigned long long ,unsigned long );

struct Int_Type5 
{
  int *ptr;
  unsigned long long addr;
}
;
unsigned int *Deref(struct UnsignedInt_Type1 );
struct UnsignedInt_Type1 UnsignedInt_Type1_Increment_UnsignedInt_Type1(unsigned long long );
bool b_LessThan_UnsignedInt_Type1_UnsignedInt_Type1(struct UnsignedInt_Type1 ,struct UnsignedInt_Type1 );
struct UnsignedInt_Type1 UnsignedInt_Type1_Add_UnsignedInt_Type1_i(struct UnsignedInt_Type1 ,int );
struct UnsignedInt_Type1 UnsignedInt_Type1_Assign_UnsignedInt_Type1_UnsignedInt_Type1(unsigned long long ,struct UnsignedInt_Type1 );
struct UnsignedInt_Type1 create_struct(unsigned int *,unsigned long long );
struct UnsignedInt_Type1 Assign(struct UnsignedInt_Type1 *,struct UnsignedInt_Type1 );
struct UnsignedInt_Type1 UnsignedInt_Type1_Cast_Void_Type0_PntrArr(struct Void_Type0 );

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

struct node 
{
  struct node *prev;
  struct node *next;
  unsigned int val;
//unsigned int array1[PTR2_SIZE];
}
;

int main()
{
  StartClock();
  unsigned int *no_init_ptr;
  unsigned int *ptr;
  struct UnsignedInt_Type1 ptr_str;
  ((ptr_str.ptr = ptr , ptr_str.addr = (reinterpret_cast < unsigned long long  >  ((&ptr)))) , ((Assign(&ptr_str,UnsignedInt_Type1_Cast_Void_Type0_PntrArr(malloc_overload(400UL))) , ptr = ptr_str.ptr)));
  unsigned int *ptr2;
  struct UnsignedInt_Type1 ptr2_str;
  ((ptr2_str.ptr = ptr2 , ptr2_str.addr = (reinterpret_cast < unsigned long long  >  ((&ptr2)))) , ((Assign(&ptr2_str,UnsignedInt_Type1_Cast_Void_Type0_PntrArr(malloc_overload(40UL))) , ptr2 = ptr2_str.ptr)));
  unsigned int *ptr_index;
  unsigned int counter = 0U;
  struct UnsignedInt_Type1 UnsignedInt_Type1_ovl_2;
  struct UnsignedInt_Type1 UnsignedInt_Type1_ovl_3;
  for ((((UnsignedInt_Type1_ovl_2 = create_struct(ptr_index,(reinterpret_cast < unsigned long long  >  ((&ptr_index)))) , UnsignedInt_Type1_Assign_UnsignedInt_Type1_UnsignedInt_Type1((reinterpret_cast < unsigned long long  >  ((&UnsignedInt_Type1_ovl_2))),create_struct(ptr,(reinterpret_cast < unsigned long long  >  ((&ptr)))))) ,  *(reinterpret_cast < void ** >  (UnsignedInt_Type1_ovl_2.addr)) = UnsignedInt_Type1_ovl_2.ptr) , UnsignedInt_Type1_ovl_2); b_LessThan_UnsignedInt_Type1_UnsignedInt_Type1(create_struct(ptr_index,(reinterpret_cast < unsigned long long  >  ((&ptr_index)))),UnsignedInt_Type1_Add_UnsignedInt_Type1_i(create_struct(ptr,(reinterpret_cast < unsigned long long  >  ((&ptr)))),100)); (((UnsignedInt_Type1_ovl_3 = create_struct(ptr_index,(reinterpret_cast < unsigned long long  >  ((&ptr_index)))) , UnsignedInt_Type1_Increment_UnsignedInt_Type1((reinterpret_cast < unsigned long long  >  ((&UnsignedInt_Type1_ovl_3))))) ,  *(reinterpret_cast < void ** >  (UnsignedInt_Type1_ovl_3.addr)) = UnsignedInt_Type1_ovl_3.ptr) , UnsignedInt_Type1_ovl_3)) {
     *Deref(create_struct(ptr_index,(reinterpret_cast < unsigned long long  >  ((&ptr_index))))) = counter++;
  }
  unsigned long array_index4 = 10UL;
  int array[array_index4];
  for (counter = 0U; counter < 100U; counter++) {
     *__Pb__i__Pe___PntrArrRef_Int_Type5_Ui(create_struct(((int *)array),(reinterpret_cast < unsigned long long  >  ((&array))),array_index4),counter) = counter;
  }
  printf("%u\n", *Deref(create_struct(((int *)array),(reinterpret_cast < unsigned long long  >  ((&array))),array_index4)));
  printf("%u\n", *__Pb__i__Pe___PntrArrRef_Int_Type5_i(create_struct(((int *)array),(reinterpret_cast < unsigned long long  >  ((&array))),array_index4),0));
  printf("%u\n", *Deref(create_struct(((int *)array),(reinterpret_cast < unsigned long long  >  ((&array))),array_index4)));
  printf("Done\n");
  struct UnsignedInt_Type1 UnsignedInt_Type1_ovl_6;
  struct UnsignedInt_Type1 UnsignedInt_Type1_ovl_7;
  for ((((UnsignedInt_Type1_ovl_6 = create_struct(ptr_index,(reinterpret_cast < unsigned long long  >  ((&ptr_index)))) , UnsignedInt_Type1_Assign_UnsignedInt_Type1_UnsignedInt_Type1((reinterpret_cast < unsigned long long  >  ((&UnsignedInt_Type1_ovl_6))),UnsignedInt_Type1_Sub_UnsignedInt_Type1_i(UnsignedInt_Type1_Add_UnsignedInt_Type1_i(create_struct(ptr,(reinterpret_cast < unsigned long long  >  ((&ptr)))),100),1))) ,  *(reinterpret_cast < void ** >  (UnsignedInt_Type1_ovl_6.addr)) = UnsignedInt_Type1_ovl_6.ptr) , UnsignedInt_Type1_ovl_6); b_GreaterOrEqual_UnsignedInt_Type1_UnsignedInt_Type1(create_struct(ptr_index,(reinterpret_cast < unsigned long long  >  ((&ptr_index)))),create_struct(ptr,(reinterpret_cast < unsigned long long  >  ((&ptr))))); (((UnsignedInt_Type1_ovl_7 = create_struct(ptr_index,(reinterpret_cast < unsigned long long  >  ((&ptr_index)))) , UnsignedInt_Type1_Decrement_UnsignedInt_Type1((reinterpret_cast < unsigned long long  >  ((&UnsignedInt_Type1_ovl_7))))) ,  *(reinterpret_cast < void ** >  (UnsignedInt_Type1_ovl_7.addr)) = UnsignedInt_Type1_ovl_7.ptr) , UnsignedInt_Type1_ovl_7)) {
    printf("%u\n", *Deref(create_struct(ptr_index,(reinterpret_cast < unsigned long long  >  ((&ptr_index))))));
  }
  EndClock();
  return 1;
}
