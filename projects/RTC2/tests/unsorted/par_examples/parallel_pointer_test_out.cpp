#include <cstdio>
#include <cstdlib>
#define ARR_SIZE 100
void v_Ret_array_bound_check_Ui_Arg_Ui_Arg(unsigned int ,unsigned int );

struct __Pb__v__Pe___Type 
{
  void *ptr;
  unsigned long long addr;
}
;
struct __Pb__v__Pe___Type __Pb__v__Pe___Type_Ret_malloc_overload_Ul_Arg(unsigned long );

struct __Pb__i__Pe___Type 
{
  int *ptr;
  unsigned long long addr;
}
;
static struct __Pb__i__Pe___Type __Pb__i__Pe___Type_Ret_Cast___Pb__v__Pe___Type_Arg(struct __Pb__v__Pe___Type );

static struct __Pb__i__Pe___Type __Pb__i__Pe___Type_Ret_Cast___Pb__v__Pe___Type_Arg(struct __Pb__v__Pe___Type input1)
{
  struct __Pb__i__Pe___Type output;
  output.__Pb__i__Pe___Type::ptr = ((int *)input1.__Pb__v__Pe___Type::ptr);
  output.__Pb__i__Pe___Type::addr = input1.__Pb__v__Pe___Type::addr;
  return output;
}
static struct __Pb__i__Pe___Type __Pb__i__Pe___Type_Ret_assign_and_copy___Pb____Pb__i__Pe____Pe___Arg___Pb__i__Pe___Type_Arg(int **,struct __Pb__i__Pe___Type );
void v_Ret_create_entry_UL_Arg_UL_Arg(unsigned long long ,unsigned long long );
static struct __Pb__i__Pe___Type __Pb__i__Pe___Type_Ret_create_struct___Pb__i__Pe___Arg_UL_Arg(int *,unsigned long long );

static struct __Pb__i__Pe___Type __Pb__i__Pe___Type_Ret_create_struct___Pb__i__Pe___Arg_UL_Arg(int *input1,unsigned long long input2)
{
  struct __Pb__i__Pe___Type output;
  output.__Pb__i__Pe___Type::ptr = input1;
  output.__Pb__i__Pe___Type::addr = input2;
  return output;
}

static struct __Pb__i__Pe___Type __Pb__i__Pe___Type_Ret_assign_and_copy___Pb____Pb__i__Pe____Pe___Arg___Pb__i__Pe___Type_Arg(int **input1,struct __Pb__i__Pe___Type input2)
{
   *input1 = input2.__Pb__i__Pe___Type::ptr;
  v_Ret_create_entry_UL_Arg_UL_Arg(((unsigned long long )input1),((unsigned long long )input2.__Pb__i__Pe___Type::addr));
  return __Pb__i__Pe___Type_Ret_create_struct___Pb__i__Pe___Arg_UL_Arg(input2.__Pb__i__Pe___Type::ptr,((unsigned long long )input1));
}
static struct __Pb__v__Pe___Type __Pb__v__Pe___Type_Ret_Cast___Pb__i__Pe___Arg_UL_Arg(int *,unsigned long long );

static struct __Pb__v__Pe___Type __Pb__v__Pe___Type_Ret_Cast___Pb__i__Pe___Arg_UL_Arg(int *input1,unsigned long long input2)
{
  struct __Pb__v__Pe___Type output;
  output.__Pb__v__Pe___Type::ptr = ((void *)input1);
  output.__Pb__v__Pe___Type::addr = input2;
  return output;
}
void v_Ret_free_overload___Pb__v__Pe___Type_Arg(struct __Pb__v__Pe___Type );
void v_Ret_execAtFirst();
void v_Ret_execAtLast();

int main()
{
  v_Ret_execAtFirst();
  int *arr[100UL];
  for (int index = 0; index < 100; index++) {
    int i_index_0;
    (i_index_0 = index , v_Ret_array_bound_check_Ui_Arg_Ui_Arg(sizeof(arr) / sizeof(int *),((unsigned int )i_index_0)));
    __Pb__i__Pe___Type_Ret_assign_and_copy___Pb____Pb__i__Pe____Pe___Arg___Pb__i__Pe___Type_Arg(&arr[i_index_0],__Pb__i__Pe___Type_Ret_Cast___Pb__v__Pe___Type_Arg(__Pb__v__Pe___Type_Ret_malloc_overload_Ul_Arg(((unsigned long )400UL))));
  }
  for (int index = 0; index < 100; index++) {
    int i_index_1;
    (i_index_1 = index , v_Ret_array_bound_check_Ui_Arg_Ui_Arg(sizeof(arr) / sizeof(int *),((unsigned int )i_index_1)));
    v_Ret_free_overload___Pb__v__Pe___Type_Arg(__Pb__v__Pe___Type_Ret_Cast___Pb__i__Pe___Arg_UL_Arg(arr[i_index_1],((unsigned long long )(&arr[i_index_1]))));
  }
  v_Ret_execAtLast();
  return 0;
}
