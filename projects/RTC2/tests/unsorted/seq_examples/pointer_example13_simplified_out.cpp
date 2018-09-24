#include "foo2_c.h" 
struct UnsignedInt_Type1 create_struct(unsigned int [array2_index3],unsigned long long ,unsigned long );
unsigned int Ui_PntrArrRef_UnsignedInt_Type1_i(struct UnsignedInt_Type1 ,int );
struct UnsignedInt_Type1 create_struct(unsigned int [array1_index2],unsigned long long ,unsigned long );
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
//unsigned int *ptr = (unsigned int*)malloc(PTR_SIZE*sizeof(int));
  unsigned int *ptr2;
  struct UnsignedInt_Type1 ptr2_str;
  ((ptr2_str.ptr = ptr2 , ptr2_str.addr = (reinterpret_cast < unsigned long long  >  ((&ptr2)))) , ((Assign(&ptr2_str,UnsignedInt_Type1_Cast_Void_Type0(malloc_overload(40UL))) , ptr2 = ptr2_str.ptr)));
  unsigned long array1_index2 = 100UL;
  unsigned int array1[array1_index2];
  unsigned long array2_index3 = 10UL;
  unsigned int array2[array2_index3];
  unsigned long array_ptr_index4 = 100UL;
  unsigned int *array_ptr[array_ptr_index4];
  unsigned long array_2d_index5 = 100UL;
  unsigned int array_2d[array_2d_index5][array2_index3];
  Ui_PntrArrRef_UnsignedInt_Type1_i(create_struct(array1,(reinterpret_cast < unsigned long long  >  ((&array1))),array1_index2),25) = Ui_PntrArrRef_UnsignedInt_Type1_i(create_struct(array2,(reinterpret_cast < unsigned long long  >  ((&array2))),array2_index3),5)++;
  EndClock();
  return 1;
}
