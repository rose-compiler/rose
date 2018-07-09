#include "foo2_c.h" 
struct UnsignedInt_Type1 UnsignedInt_Type1_Decrement_UnsignedInt_Type1(unsigned long long );
bool b_GreaterOrEqual_UnsignedInt_Type1_UnsignedInt_Type1(struct UnsignedInt_Type1 ,struct UnsignedInt_Type1 );
struct UnsignedInt_Type1 UnsignedInt_Type1_Sub_UnsignedInt_Type1_i(struct UnsignedInt_Type1 ,int );
struct UnsignedInt_Type1 UnsignedInt_Type1_Increment_UnsignedInt_Type1(unsigned long long );
bool b_LessThan_UnsignedInt_Type1_UnsignedInt_Type1(struct UnsignedInt_Type1 ,struct UnsignedInt_Type1 );
struct UnsignedInt_Type1 UnsignedInt_Type1_Add_UnsignedInt_Type1_i(struct UnsignedInt_Type1 ,int );
struct UnsignedInt_Type1 UnsignedInt_Type1_Assign_UnsignedInt_Type1_UnsignedInt_Type1(unsigned long long ,struct UnsignedInt_Type1 );
struct UnsignedInt_Type1 create_struct(unsigned int *,unsigned long long );
struct UnsignedInt_Type1 UnsignedInt_Type1_Cast_i_PntrArr_ValCast(int );
struct UnsignedInt_Type1 UnsignedInt_Type1_Cast_Void_Type0_PntrArr(struct Void_Type0 );
struct Void_Type0 malloc_overload(int );
unsigned int *Deref(struct UnsignedInt_Type1 );
struct UnsignedInt_Type1 Assign(struct UnsignedInt_Type1 *,struct UnsignedInt_Type1 );
struct Char_Type4 Char_Type4_Cast_UnsignedInt_Type1_PntrArr(struct UnsignedInt_Type1 );
struct Double_Type3 Double_Type3_Cast_UnsignedInt_Type1_PntrArr(struct UnsignedInt_Type1 );
struct Float_Type2 Float_Type2_Cast_UnsignedInt_Type1_PntrArr(struct UnsignedInt_Type1 );
struct Void_Type0 Void_Type0_Cast_UnsignedInt_Type1_PntrArr(struct UnsignedInt_Type1 );
void create_entry(unsigned long long ,unsigned long long );

struct Char_Type4 
{
  char *ptr;
  unsigned long long addr;
}
;

struct Double_Type3 
{
  double *ptr;
  unsigned long long addr;
}
;

struct Float_Type2 
{
  float *ptr;
  unsigned long long addr;
}
;

struct UnsignedInt_Type1 
{
  unsigned int *ptr;
  unsigned long long addr;
}
;

struct Void_Type0 
{
  void *ptr;
  unsigned long long addr;
}
;
struct UnsignedInt_Type1 fn2(struct Void_Type0 voidin_str,struct UnsignedInt_Type1 ,struct Float_Type2 ,struct Double_Type3 ,struct Char_Type4 );
void print_array(struct UnsignedInt_Type1 array_str,unsigned int size);

struct UnsignedInt_Type1 fn1(struct UnsignedInt_Type1 input_str)
{
  (create_entry((reinterpret_cast < unsigned long long  >  ((&input_str.ptr))),input_str.addr) , input_str.addr = (reinterpret_cast < unsigned long long  >  ((&input_str.ptr))));
  unsigned int *nullptr;
  struct UnsignedInt_Type1 nullptr_str;
  ((nullptr_str.ptr = nullptr , nullptr_str.addr = (reinterpret_cast < unsigned long long  >  ((&nullptr)))) , ((Assign(&nullptr_str,::fn2(Void_Type0_Cast_UnsignedInt_Type1_PntrArr(input_str),input_str,Float_Type2_Cast_UnsignedInt_Type1_PntrArr(input_str),Double_Type3_Cast_UnsignedInt_Type1_PntrArr(input_str),Char_Type4_Cast_UnsignedInt_Type1_PntrArr(input_str))) , nullptr = nullptr_str.ptr)));
  printf("input: %u\n", *Deref(input_str));
  return UnsignedInt_Type1_Cast_Void_Type0_PntrArr(malloc_overload(400U));
}

struct UnsignedInt_Type1 fn2(struct Void_Type0 voidin_str,struct UnsignedInt_Type1 input_str,struct Float_Type2 input2_str,struct Double_Type3 input3_str,struct Char_Type4 input4_str)
{
  (create_entry((reinterpret_cast < unsigned long long  >  ((&input4_str.ptr))),input4_str.addr) , input4_str.addr = (reinterpret_cast < unsigned long long  >  ((&input4_str.ptr))));
  (create_entry((reinterpret_cast < unsigned long long  >  ((&input3_str.ptr))),input3_str.addr) , input3_str.addr = (reinterpret_cast < unsigned long long  >  ((&input3_str.ptr))));
  (create_entry((reinterpret_cast < unsigned long long  >  ((&input2_str.ptr))),input2_str.addr) , input2_str.addr = (reinterpret_cast < unsigned long long  >  ((&input2_str.ptr))));
  (create_entry((reinterpret_cast < unsigned long long  >  ((&input_str.ptr))),input_str.addr) , input_str.addr = (reinterpret_cast < unsigned long long  >  ((&input_str.ptr))));
  (create_entry((reinterpret_cast < unsigned long long  >  ((&voidin_str.ptr))),voidin_str.addr) , voidin_str.addr = (reinterpret_cast < unsigned long long  >  ((&voidin_str.ptr))));
  return UnsignedInt_Type1_Cast_i_PntrArr_ValCast(0);
}

int main()
{
  StartClock();
  unsigned int *no_init_ptr;
  unsigned int *ptr;
  struct UnsignedInt_Type1 ptr_str;
  ((ptr_str.Void_Type0::ptr = ptr , ptr_str.addr = (reinterpret_cast < unsigned long long  >  ((&ptr)))) , ((Assign(&ptr_str,UnsignedInt_Type1_Cast_Void_Type0_PntrArr(malloc_overload(400U))) , ptr = ptr_str.Void_Type0::ptr)));
  unsigned int *ptr2;
  struct UnsignedInt_Type1 ptr2_str;
  ((ptr2_str.Void_Type0::ptr = ptr2 , ptr2_str.addr = (reinterpret_cast < unsigned long long  >  ((&ptr2)))) , ((Assign(&ptr2_str,UnsignedInt_Type1_Cast_Void_Type0_PntrArr(malloc_overload(40U))) , ptr2 = ptr2_str.Void_Type0::ptr)));
  unsigned int *ptr_index;
  unsigned int counter = 0U;
  struct UnsignedInt_Type1 UnsignedInt_Type1_ovl_5;
  struct UnsignedInt_Type1 UnsignedInt_Type1_ovl_6;
  for ((((UnsignedInt_Type1_ovl_5 = create_struct(ptr_index,(reinterpret_cast < unsigned long long  >  ((&ptr_index)))) , UnsignedInt_Type1_Assign_UnsignedInt_Type1_UnsignedInt_Type1((reinterpret_cast < unsigned long long  >  ((&UnsignedInt_Type1_ovl_5))),ptr_str)) ,  *(reinterpret_cast < void ** >  (UnsignedInt_Type1_ovl_5.addr)) = UnsignedInt_Type1_ovl_5.Void_Type0::ptr) , UnsignedInt_Type1_ovl_5); b_LessThan_UnsignedInt_Type1_UnsignedInt_Type1(create_struct(ptr_index,(reinterpret_cast < unsigned long long  >  ((&ptr_index)))),UnsignedInt_Type1_Add_UnsignedInt_Type1_i(ptr_str,100)); (((UnsignedInt_Type1_ovl_6 = create_struct(ptr_index,(reinterpret_cast < unsigned long long  >  ((&ptr_index)))) , UnsignedInt_Type1_Increment_UnsignedInt_Type1((reinterpret_cast < unsigned long long  >  ((&UnsignedInt_Type1_ovl_6))))) ,  *(reinterpret_cast < void ** >  (UnsignedInt_Type1_ovl_6.addr)) = UnsignedInt_Type1_ovl_6.Void_Type0::ptr) , UnsignedInt_Type1_ovl_6)) {
     *Deref(create_struct(ptr_index,(reinterpret_cast < unsigned long long  >  ((&ptr_index))))) = counter++;
  }
  struct UnsignedInt_Type1 UnsignedInt_Type1_ovl_7;
  struct UnsignedInt_Type1 UnsignedInt_Type1_ovl_8;
  for ((((UnsignedInt_Type1_ovl_7 = create_struct(ptr_index,(reinterpret_cast < unsigned long long  >  ((&ptr_index)))) , UnsignedInt_Type1_Assign_UnsignedInt_Type1_UnsignedInt_Type1((reinterpret_cast < unsigned long long  >  ((&UnsignedInt_Type1_ovl_7))),UnsignedInt_Type1_Sub_UnsignedInt_Type1_i(UnsignedInt_Type1_Add_UnsignedInt_Type1_i(ptr_str,100),1))) ,  *(reinterpret_cast < void ** >  (UnsignedInt_Type1_ovl_7.addr)) = UnsignedInt_Type1_ovl_7.Void_Type0::ptr) , UnsignedInt_Type1_ovl_7); b_GreaterOrEqual_UnsignedInt_Type1_UnsignedInt_Type1(create_struct(ptr_index,(reinterpret_cast < unsigned long long  >  ((&ptr_index)))),ptr_str); (((UnsignedInt_Type1_ovl_8 = create_struct(ptr_index,(reinterpret_cast < unsigned long long  >  ((&ptr_index)))) , UnsignedInt_Type1_Decrement_UnsignedInt_Type1((reinterpret_cast < unsigned long long  >  ((&UnsignedInt_Type1_ovl_8))))) ,  *(reinterpret_cast < void ** >  (UnsignedInt_Type1_ovl_8.addr)) = UnsignedInt_Type1_ovl_8.Void_Type0::ptr) , UnsignedInt_Type1_ovl_8)) {
    printf("%u\n", *Deref(create_struct(ptr_index,(reinterpret_cast < unsigned long long  >  ((&ptr_index))))));
  }
  EndClock();
  return 1;
}
