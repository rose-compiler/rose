#include "foo2_c.h" 
struct UnsignedInt_Type9 UnsignedInt_Type9_Decrement_UnsignedInt_Type9(unsigned long long );
bool b_GreaterOrEqual_UnsignedInt_Type9_UnsignedInt_Type9(struct UnsignedInt_Type9 ,struct UnsignedInt_Type9 );
struct UnsignedInt_Type9 UnsignedInt_Type9_Sub_UnsignedInt_Type9_i(struct UnsignedInt_Type9 ,int );
struct Class_Type0 create_struct(struct node *,unsigned long long );
struct Class_Type0 Assign(struct Class_Type0 *,struct Class_Type0 );
struct Class_Type0 Class_Type0_Cast_Void_Type8_PntrArr(struct Void_Type8 );
struct Char_Type12 Char_Type12_Cast_i_PntrArr_ValCast(int );
struct Float_Type10 Float_Type10_Cast_Class_Type0_PntrArr(struct Class_Type0 );
struct UnsignedInt_Type9 UnsignedInt_Type9_Increment_UnsignedInt_Type9(unsigned long long );
bool b_LessThan_UnsignedInt_Type9_UnsignedInt_Type9(struct UnsignedInt_Type9 ,struct UnsignedInt_Type9 );
struct UnsignedInt_Type9 UnsignedInt_Type9_Add_UnsignedInt_Type9_i(struct UnsignedInt_Type9 ,int );
struct UnsignedInt_Type9 UnsignedInt_Type9_Assign_UnsignedInt_Type9_UnsignedInt_Type9(unsigned long long ,struct UnsignedInt_Type9 );
struct UnsignedInt_Type9 create_struct(unsigned int *,unsigned long long );
struct UnsignedInt_Type9 UnsignedInt_Type9_Cast_i_PntrArr_ValCast(int );
struct UnsignedInt_Type9 UnsignedInt_Type9_Cast_Void_Type8_PntrArr(struct Void_Type8 );
struct Void_Type8 malloc_overload(int );
unsigned int *Deref(struct UnsignedInt_Type9 );
struct UnsignedInt_Type9 Assign(struct UnsignedInt_Type9 *,struct UnsignedInt_Type9 );
struct Char_Type12 Char_Type12_Cast_UnsignedInt_Type9_PntrArr(struct UnsignedInt_Type9 );
struct Double_Type11 Double_Type11_Cast_UnsignedInt_Type9_PntrArr(struct UnsignedInt_Type9 );
struct Float_Type10 Float_Type10_Cast_UnsignedInt_Type9_PntrArr(struct UnsignedInt_Type9 );
struct Void_Type8 Void_Type8_Cast_UnsignedInt_Type9_PntrArr(struct UnsignedInt_Type9 );

struct Char_Type12 
{
  char *ptr;
  unsigned long long addr;
}
;

struct Double_Type11 
{
  double *ptr;
  unsigned long long addr;
}
;

struct Float_Type10 
{
  float *ptr;
  unsigned long long addr;
}
;

struct UnsignedInt_Type9 
{
  unsigned int *ptr;
  unsigned long long addr;
}
;
struct Void_Type8 Void_Type8_Cast_Class_Type0_PntrArr(struct Class_Type0 );

struct Void_Type8 
{
  void *ptr;
  unsigned long long addr;
}
;
struct Class_Type0 Class_Type0_DotExp_node___Pb__node__Pe__(struct node *,unsigned long long );
struct Class_Type0 Class_Type0_Assign_Class_Type0_Class_Type0(unsigned long long ,struct Class_Type0 );
struct Class_Type0 Class_Type0_ArrowExp___Pb__node__Pe_____Pb__node__Pe__(struct node *,unsigned long long );
struct node *Deref(struct Class_Type0 );
struct Char_Type1 create_struct(const char [12UL],unsigned long long ,unsigned long );

struct Char_Type1 
{
  const char *ptr;
  unsigned long long addr;
}
;
bool b_NotEqual_Class_Type0_Class_Type0(struct Class_Type0 ,struct Class_Type0 );
struct Class_Type0 Class_Type0_Cast_i_PntrArr_ValCast(int );
void create_entry(unsigned long long ,unsigned long long );

struct Class_Type0 
{
  struct node *ptr;
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

void insert_node(struct Class_Type0 insert_str,struct Class_Type0 after_str)
{
  (create_entry((reinterpret_cast < unsigned long long  >  ((&after_str.ptr))),after_str.addr) , after_str.addr = (reinterpret_cast < unsigned long long  >  ((&after_str.ptr))));
  (create_entry((reinterpret_cast < unsigned long long  >  ((&insert_str.ptr))),insert_str.addr) , insert_str.addr = (reinterpret_cast < unsigned long long  >  ((&insert_str.ptr))));
  (__builtin_expect((!b_NotEqual_Class_Type0_Class_Type0(insert_str,Class_Type0_Cast_i_PntrArr_ValCast(0))),0L))?__assert_rtn(create_struct(__func__,(reinterpret_cast < unsigned long long  >  ((&__func__))),12UL),"/Users/vanka1/research/compilers/rose_public/rose_build/projects/RTC/pointer_example16_simplified.cpp",23,"insert != NULL") : ((void )0);
  (__builtin_expect((!b_NotEqual_Class_Type0_Class_Type0(after_str,Class_Type0_Cast_i_PntrArr_ValCast(0))),0L))?__assert_rtn(create_struct(__func__,(reinterpret_cast < unsigned long long  >  ((&__func__))),12UL),"/Users/vanka1/research/compilers/rose_public/rose_build/projects/RTC/pointer_example16_simplified.cpp",24,"after != NULL") : ((void )0);
// start: after -> second
// start: after <- second
// after -> insert -> second
  struct Class_Type0 Class_Type0_ovl_2;
  (((Class_Type0_ovl_2 = Class_Type0_ArrowExp___Pb__node__Pe_____Pb__node__Pe__(Deref(insert_str) -> node::next,(reinterpret_cast < unsigned long long  >  ((&Deref(insert_str) -> node::next)))) , Class_Type0_Assign_Class_Type0_Class_Type0((reinterpret_cast < unsigned long long  >  ((&Class_Type0_ovl_2))),Class_Type0_ArrowExp___Pb__node__Pe_____Pb__node__Pe__(Deref(after_str) -> node::next,(reinterpret_cast < unsigned long long  >  ((&Deref(after_str) -> node::next)))))) ,  *(reinterpret_cast < struct node ** >  (Class_Type0_ovl_2.addr)) = Class_Type0_ovl_2.ptr) , Class_Type0_ovl_2);
  struct Class_Type0 Class_Type0_ovl_3;
  (((Class_Type0_ovl_3 = Class_Type0_ArrowExp___Pb__node__Pe_____Pb__node__Pe__(Deref(after_str) -> node::next,(reinterpret_cast < unsigned long long  >  ((&Deref(after_str) -> node::next)))) , Class_Type0_Assign_Class_Type0_Class_Type0((reinterpret_cast < unsigned long long  >  ((&Class_Type0_ovl_3))),insert_str)) ,  *(reinterpret_cast < struct node ** >  (Class_Type0_ovl_3.addr)) = Class_Type0_ovl_3.ptr) , Class_Type0_ovl_3);
// after <- insert
  struct Class_Type0 Class_Type0_ovl_4;
  (((Class_Type0_ovl_4 = Class_Type0_ArrowExp___Pb__node__Pe_____Pb__node__Pe__(Deref(insert_str) -> node::prev,(reinterpret_cast < unsigned long long  >  ((&Deref(insert_str) -> node::prev)))) , Class_Type0_Assign_Class_Type0_Class_Type0((reinterpret_cast < unsigned long long  >  ((&Class_Type0_ovl_4))),after_str)) ,  *(reinterpret_cast < struct node ** >  (Class_Type0_ovl_4.addr)) = Class_Type0_ovl_4.ptr) , Class_Type0_ovl_4);
// insert <- second
  if (b_NotEqual_Class_Type0_Class_Type0(Class_Type0_ArrowExp___Pb__node__Pe_____Pb__node__Pe__(Deref(insert_str) -> node::next,(reinterpret_cast < unsigned long long  >  ((&Deref(insert_str) -> node::next)))),Class_Type0_Cast_i_PntrArr_ValCast(0))) {
    struct Class_Type0 Class_Type0_ovl_5;
    (((Class_Type0_ovl_5 = Class_Type0_DotExp_node___Pb__node__Pe__(( *Deref(Class_Type0_ArrowExp___Pb__node__Pe_____Pb__node__Pe__(Deref(insert_str) -> node::next,(reinterpret_cast < unsigned long long  >  ((&Deref(insert_str) -> node::next)))))).node::prev,(reinterpret_cast < unsigned long long  >  ((&( *Deref(Class_Type0_ArrowExp___Pb__node__Pe_____Pb__node__Pe__(Deref(insert_str) -> node::next,(reinterpret_cast < unsigned long long  >  ((&Deref(insert_str) -> node::next)))))).node::prev)))) , Class_Type0_Assign_Class_Type0_Class_Type0((reinterpret_cast < unsigned long long  >  ((&Class_Type0_ovl_5))),insert_str)) ,  *(reinterpret_cast < struct node ** >  (Class_Type0_ovl_5.addr)) = Class_Type0_ovl_5.ptr) , Class_Type0_ovl_5);
  }
}

void delete_node(struct Class_Type0 to_del_str)
{
  (create_entry((reinterpret_cast < unsigned long long  >  ((&to_del_str.ptr))),to_del_str.addr) , to_del_str.addr = (reinterpret_cast < unsigned long long  >  ((&to_del_str.ptr))));
  (__builtin_expect((!b_NotEqual_Class_Type0_Class_Type0(to_del_str,Class_Type0_Cast_i_PntrArr_ValCast(0))),0L))?__assert_rtn(create_struct(__func__,(reinterpret_cast < unsigned long long  >  ((&__func__))),12UL),"/Users/vanka1/research/compilers/rose_public/rose_build/projects/RTC/pointer_example16_simplified.cpp",44,"to_del != NULL") : ((void )0);
// start: first -> to_del -> second
// start: first <- to_del <- second
// first -> second
  struct Class_Type0 Class_Type0_ovl_6;
  (((Class_Type0_ovl_6 = Class_Type0_DotExp_node___Pb__node__Pe__(( *Deref(Class_Type0_ArrowExp___Pb__node__Pe_____Pb__node__Pe__(Deref(to_del_str) -> node::prev,(reinterpret_cast < unsigned long long  >  ((&Deref(to_del_str) -> node::prev)))))).node::next,(reinterpret_cast < unsigned long long  >  ((&( *Deref(Class_Type0_ArrowExp___Pb__node__Pe_____Pb__node__Pe__(Deref(to_del_str) -> node::prev,(reinterpret_cast < unsigned long long  >  ((&Deref(to_del_str) -> node::prev)))))).node::next)))) , Class_Type0_Assign_Class_Type0_Class_Type0((reinterpret_cast < unsigned long long  >  ((&Class_Type0_ovl_6))),Class_Type0_ArrowExp___Pb__node__Pe_____Pb__node__Pe__(Deref(to_del_str) -> node::next,(reinterpret_cast < unsigned long long  >  ((&Deref(to_del_str) -> node::next)))))) ,  *(reinterpret_cast < struct node ** >  (Class_Type0_ovl_6.addr)) = Class_Type0_ovl_6.ptr) , Class_Type0_ovl_6);
// first <- second
  struct Class_Type0 Class_Type0_ovl_7;
  (((Class_Type0_ovl_7 = Class_Type0_DotExp_node___Pb__node__Pe__(( *Deref(Class_Type0_ArrowExp___Pb__node__Pe_____Pb__node__Pe__(Deref(to_del_str) -> node::next,(reinterpret_cast < unsigned long long  >  ((&Deref(to_del_str) -> node::next)))))).node::prev,(reinterpret_cast < unsigned long long  >  ((&( *Deref(Class_Type0_ArrowExp___Pb__node__Pe_____Pb__node__Pe__(Deref(to_del_str) -> node::next,(reinterpret_cast < unsigned long long  >  ((&Deref(to_del_str) -> node::next)))))).node::prev)))) , Class_Type0_Assign_Class_Type0_Class_Type0((reinterpret_cast < unsigned long long  >  ((&Class_Type0_ovl_7))),Class_Type0_ArrowExp___Pb__node__Pe_____Pb__node__Pe__(Deref(to_del_str) -> node::prev,(reinterpret_cast < unsigned long long  >  ((&Deref(to_del_str) -> node::prev)))))) ,  *(reinterpret_cast < struct node ** >  (Class_Type0_ovl_7.addr)) = Class_Type0_ovl_7.ptr) , Class_Type0_ovl_7);
  free(Void_Type8_Cast_Class_Type0_PntrArr(to_del_str));
}
struct UnsignedInt_Type9 fn2(struct Void_Type8 voidin_str,struct UnsignedInt_Type9 ,struct Float_Type10 ,struct Double_Type11 ,struct Char_Type12 );
void print_array(struct UnsignedInt_Type9 array_str,unsigned int size);

struct UnsignedInt_Type9 fn1(struct UnsignedInt_Type9 input_str)
{
  (create_entry((reinterpret_cast < unsigned long long  >  ((&input_str.ptr))),input_str.addr) , input_str.addr = (reinterpret_cast < unsigned long long  >  ((&input_str.ptr))));
  unsigned int *nullptr;
  struct UnsignedInt_Type9 nullptr_str;
  ((nullptr_str.ptr = nullptr , nullptr_str.addr = (reinterpret_cast < unsigned long long  >  ((&nullptr)))) , ((Assign(&nullptr_str,::fn2(Void_Type8_Cast_UnsignedInt_Type9_PntrArr(input_str),input_str,Float_Type10_Cast_UnsignedInt_Type9_PntrArr(input_str),Double_Type11_Cast_UnsignedInt_Type9_PntrArr(input_str),Char_Type12_Cast_UnsignedInt_Type9_PntrArr(input_str))) , nullptr = nullptr_str.ptr)));
  printf("input: %u\n", *Deref(input_str));
  return UnsignedInt_Type9_Cast_Void_Type8_PntrArr(malloc_overload(400UL));
}

struct UnsignedInt_Type9 fn2(struct Void_Type8 voidin_str,struct UnsignedInt_Type9 input_str,struct Float_Type10 input2_str,struct Double_Type11 input3_str,struct Char_Type12 input4_str)
{
  (create_entry((reinterpret_cast < unsigned long long  >  ((&input4_str.ptr))),input4_str.addr) , input4_str.addr = (reinterpret_cast < unsigned long long  >  ((&input4_str.ptr))));
  (create_entry((reinterpret_cast < unsigned long long  >  ((&input3_str.ptr))),input3_str.addr) , input3_str.addr = (reinterpret_cast < unsigned long long  >  ((&input3_str.ptr))));
  (create_entry((reinterpret_cast < unsigned long long  >  ((&input2_str.ptr))),input2_str.addr) , input2_str.addr = (reinterpret_cast < unsigned long long  >  ((&input2_str.ptr))));
  (create_entry((reinterpret_cast < unsigned long long  >  ((&input_str.ptr))),input_str.addr) , input_str.addr = (reinterpret_cast < unsigned long long  >  ((&input_str.ptr))));
  (create_entry((reinterpret_cast < unsigned long long  >  ((&voidin_str.ptr))),voidin_str.addr) , voidin_str.addr = (reinterpret_cast < unsigned long long  >  ((&voidin_str.ptr))));
  return UnsignedInt_Type9_Cast_i_PntrArr_ValCast(0);
}

int main()
{
  StartClock();
  unsigned int *no_init_ptr;
  unsigned int *ptr;
  struct UnsignedInt_Type9 ptr_str;
  ((ptr_str.Class_Type0::ptr = ptr , ptr_str.addr = (reinterpret_cast < unsigned long long  >  ((&ptr)))) , ((Assign(&ptr_str,UnsignedInt_Type9_Cast_Void_Type8_PntrArr(malloc_overload(400UL))) , ptr = ptr_str.Class_Type0::ptr)));
  unsigned int *ptr2;
  struct UnsignedInt_Type9 ptr2_str;
  ((ptr2_str.Class_Type0::ptr = ptr2 , ptr2_str.addr = (reinterpret_cast < unsigned long long  >  ((&ptr2)))) , ((Assign(&ptr2_str,UnsignedInt_Type9_Cast_Void_Type8_PntrArr(malloc_overload(40UL))) , ptr2 = ptr2_str.Class_Type0::ptr)));
  unsigned int *ptr_index;
  unsigned int counter = 0U;
  struct UnsignedInt_Type9 UnsignedInt_Type9_ovl_13;
  struct UnsignedInt_Type9 UnsignedInt_Type9_ovl_14;
  for ((((UnsignedInt_Type9_ovl_13 = create_struct(ptr_index,(reinterpret_cast < unsigned long long  >  ((&ptr_index)))) , UnsignedInt_Type9_Assign_UnsignedInt_Type9_UnsignedInt_Type9((reinterpret_cast < unsigned long long  >  ((&UnsignedInt_Type9_ovl_13))),create_struct(ptr,(reinterpret_cast < unsigned long long  >  ((&ptr)))))) ,  *(reinterpret_cast < struct node ** >  (UnsignedInt_Type9_ovl_13.addr)) = UnsignedInt_Type9_ovl_13.Class_Type0::ptr) , UnsignedInt_Type9_ovl_13); b_LessThan_UnsignedInt_Type9_UnsignedInt_Type9(create_struct(ptr_index,(reinterpret_cast < unsigned long long  >  ((&ptr_index)))),UnsignedInt_Type9_Add_UnsignedInt_Type9_i(create_struct(ptr,(reinterpret_cast < unsigned long long  >  ((&ptr)))),100)); (((UnsignedInt_Type9_ovl_14 = create_struct(ptr_index,(reinterpret_cast < unsigned long long  >  ((&ptr_index)))) , UnsignedInt_Type9_Increment_UnsignedInt_Type9((reinterpret_cast < unsigned long long  >  ((&UnsignedInt_Type9_ovl_14))))) ,  *(reinterpret_cast < struct node ** >  (UnsignedInt_Type9_ovl_14.addr)) = UnsignedInt_Type9_ovl_14.Class_Type0::ptr) , UnsignedInt_Type9_ovl_14)) {
     *Deref(create_struct(ptr_index,(reinterpret_cast < unsigned long long  >  ((&ptr_index))))) = counter++;
  }
// dot exp check
  struct node dot_head;
  struct UnsignedInt_Type9 UnsignedInt_Type9_ovl_15;
  (((UnsignedInt_Type9_ovl_15 = create_struct(no_init_ptr,(reinterpret_cast < unsigned long long  >  ((&no_init_ptr)))) , UnsignedInt_Type9_Assign_UnsignedInt_Type9_UnsignedInt_Type9((reinterpret_cast < unsigned long long  >  ((&UnsignedInt_Type9_ovl_15))),::fn2(Void_Type8_Cast_Class_Type0_PntrArr(Class_Type0_DotExp_node___Pb__node__Pe__(dot_head.node::next,(reinterpret_cast < unsigned long long  >  ((&dot_head.node::next))))),create_struct(ptr,(reinterpret_cast < unsigned long long  >  ((&ptr)))),Float_Type10_Cast_Class_Type0_PntrArr(Class_Type0_DotExp_node___Pb__node__Pe__(dot_head.node::prev,(reinterpret_cast < unsigned long long  >  ((&dot_head.node::prev))))),Double_Type11_Cast_UnsignedInt_Type9_PntrArr(create_struct(ptr2,(reinterpret_cast < unsigned long long  >  ((&ptr2))))),Char_Type12_Cast_i_PntrArr_ValCast(0)))) ,  *(reinterpret_cast < struct node ** >  (UnsignedInt_Type9_ovl_15.addr)) = UnsignedInt_Type9_ovl_15.Class_Type0::ptr) , UnsignedInt_Type9_ovl_15);
// arrow exp check
  struct node *head;
  struct Class_Type0 head_str;
  ((head_str.Class_Type0::ptr = head , head_str.addr = (reinterpret_cast < unsigned long long  >  ((&head)))) , ((Assign(&head_str,Class_Type0_Cast_Void_Type8_PntrArr(malloc_overload(24UL))) , head = head_str.Class_Type0::ptr)));
  struct UnsignedInt_Type9 UnsignedInt_Type9_ovl_16;
  (((UnsignedInt_Type9_ovl_16 = create_struct(no_init_ptr,(reinterpret_cast < unsigned long long  >  ((&no_init_ptr)))) , UnsignedInt_Type9_Assign_UnsignedInt_Type9_UnsignedInt_Type9((reinterpret_cast < unsigned long long  >  ((&UnsignedInt_Type9_ovl_16))),::fn2(Void_Type8_Cast_Class_Type0_PntrArr(Class_Type0_ArrowExp___Pb__node__Pe_____Pb__node__Pe__(Deref(create_struct(head,(reinterpret_cast < unsigned long long  >  ((&head))))) -> node::next,(reinterpret_cast < unsigned long long  >  ((&Deref(create_struct(head,(reinterpret_cast < unsigned long long  >  ((&head))))) -> node::next))))),create_struct(ptr,(reinterpret_cast < unsigned long long  >  ((&ptr)))),Float_Type10_Cast_Class_Type0_PntrArr(Class_Type0_ArrowExp___Pb__node__Pe_____Pb__node__Pe__(Deref(create_struct(head,(reinterpret_cast < unsigned long long  >  ((&head))))) -> node::prev,(reinterpret_cast < unsigned long long  >  ((&Deref(create_struct(head,(reinterpret_cast < unsigned long long  >  ((&head))))) -> node::prev))))),Double_Type11_Cast_UnsignedInt_Type9_PntrArr(create_struct(ptr2,(reinterpret_cast < unsigned long long  >  ((&ptr2))))),Char_Type12_Cast_i_PntrArr_ValCast(0)))) ,  *(reinterpret_cast < struct node ** >  (UnsignedInt_Type9_ovl_16.addr)) = UnsignedInt_Type9_ovl_16.Class_Type0::ptr) , UnsignedInt_Type9_ovl_16);
    #if 0
// dot exp check
// arrow exp check
    #endif
  struct UnsignedInt_Type9 UnsignedInt_Type9_ovl_17;
  struct UnsignedInt_Type9 UnsignedInt_Type9_ovl_18;
  for ((((UnsignedInt_Type9_ovl_17 = create_struct(ptr_index,(reinterpret_cast < unsigned long long  >  ((&ptr_index)))) , UnsignedInt_Type9_Assign_UnsignedInt_Type9_UnsignedInt_Type9((reinterpret_cast < unsigned long long  >  ((&UnsignedInt_Type9_ovl_17))),UnsignedInt_Type9_Sub_UnsignedInt_Type9_i(UnsignedInt_Type9_Add_UnsignedInt_Type9_i(create_struct(ptr,(reinterpret_cast < unsigned long long  >  ((&ptr)))),100),1))) ,  *(reinterpret_cast < struct node ** >  (UnsignedInt_Type9_ovl_17.addr)) = UnsignedInt_Type9_ovl_17.Class_Type0::ptr) , UnsignedInt_Type9_ovl_17); b_GreaterOrEqual_UnsignedInt_Type9_UnsignedInt_Type9(create_struct(ptr_index,(reinterpret_cast < unsigned long long  >  ((&ptr_index)))),create_struct(ptr,(reinterpret_cast < unsigned long long  >  ((&ptr))))); (((UnsignedInt_Type9_ovl_18 = create_struct(ptr_index,(reinterpret_cast < unsigned long long  >  ((&ptr_index)))) , UnsignedInt_Type9_Decrement_UnsignedInt_Type9((reinterpret_cast < unsigned long long  >  ((&UnsignedInt_Type9_ovl_18))))) ,  *(reinterpret_cast < struct node ** >  (UnsignedInt_Type9_ovl_18.addr)) = UnsignedInt_Type9_ovl_18.Class_Type0::ptr) , UnsignedInt_Type9_ovl_18)) {
    printf("%u\n", *Deref(create_struct(ptr_index,(reinterpret_cast < unsigned long long  >  ((&ptr_index))))));
  }
  EndClock();
  return 1;
}
