#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#define PTR_SIZE 100
#define PTR2_SIZE 10
#define PTR3_SIZE 10
#define OUT_OF_BOUNDS_EXCESS 1

struct __Pb__Ui__Pe___Type 
{
  unsigned int *ptr;
  unsigned long long addr;
}
;
void v_Ret_create_entry_UL_Arg_UL_Arg(unsigned long long ,unsigned long long );
static unsigned int *__Pb__Ui__Pe___Ret_deref_check_with_str___Pb__Ui__Pe___Type_Arg(struct __Pb__Ui__Pe___Type );
static unsigned int *__Pb__Ui__Pe___Ret_deref_check___Pb__Ui__Pe___Arg_UL_Arg(unsigned int *,unsigned long long );
void v_Ret_check_entry_UL_Arg_UL_Arg(unsigned long long ,unsigned long long );
void v_Ret_null_check_UL_Arg(unsigned long long );

static unsigned int *__Pb__Ui__Pe___Ret_deref_check___Pb__Ui__Pe___Arg_UL_Arg(unsigned int *input1,unsigned long long input2)
{
  v_Ret_check_entry_UL_Arg_UL_Arg(((unsigned long long )input1),input2);
  v_Ret_null_check_UL_Arg(((unsigned long long )input1));
  return input1;
}

static unsigned int *__Pb__Ui__Pe___Ret_deref_check_with_str___Pb__Ui__Pe___Type_Arg(struct __Pb__Ui__Pe___Type input1)
{
  return __Pb__Ui__Pe___Ret_deref_check___Pb__Ui__Pe___Arg_UL_Arg(input1.__Pb__Ui__Pe___Type::ptr,((unsigned long long )input1.__Pb__Ui__Pe___Type::addr));
}

struct __Pb__v__Pe___Type 
{
  void *ptr;
  unsigned long long addr;
}
;
struct __Pb__v__Pe___Type __Pb__v__Pe___Type_Ret_malloc_overload_Ul_Arg(unsigned long );
static struct __Pb__Ui__Pe___Type __Pb__Ui__Pe___Type_Ret_Cast___Pb__v__Pe___Type_Arg(struct __Pb__v__Pe___Type );

static struct __Pb__Ui__Pe___Type __Pb__Ui__Pe___Type_Ret_Cast___Pb__v__Pe___Type_Arg(struct __Pb__v__Pe___Type input1)
{
  struct __Pb__Ui__Pe___Type output;
  output.__Pb__Ui__Pe___Type::ptr = ((unsigned int *)input1.__Pb__v__Pe___Type::ptr);
  output.__Pb__Ui__Pe___Type::addr = input1.__Pb__v__Pe___Type::addr;
  return output;
}

struct __Pb__Ui__Pe___Type fn1(struct __Pb__Ui__Pe___Type input_str,struct __Pb__Ui__Pe___Type input2_str)
{
  (v_Ret_create_entry_UL_Arg_UL_Arg(((unsigned long long )(&input2_str.__Pb__Ui__Pe___Type::ptr)),input2_str.__Pb__Ui__Pe___Type::addr) , input2_str.__Pb__Ui__Pe___Type::addr = ((unsigned long long )(&input2_str.__Pb__Ui__Pe___Type::ptr)));
  (v_Ret_create_entry_UL_Arg_UL_Arg(((unsigned long long )(&input_str.__Pb__Ui__Pe___Type::ptr)),input_str.__Pb__Ui__Pe___Type::addr) , input_str.__Pb__Ui__Pe___Type::addr = ((unsigned long long )(&input_str.__Pb__Ui__Pe___Type::ptr)));
  printf("input: %u\n", *__Pb__Ui__Pe___Ret_deref_check_with_str___Pb__Ui__Pe___Type_Arg(input_str));
  return __Pb__Ui__Pe___Type_Ret_Cast___Pb__v__Pe___Type_Arg(__Pb__v__Pe___Type_Ret_malloc_overload_Ul_Arg(((unsigned long )400UL)));
}
static struct __Pb__Ui__Pe___Type __Pb__Ui__Pe___Type_Ret_assign_and_copy___Pb____Pb__Ui__Pe____Pe___Arg___Pb__Ui__Pe___Type_Arg(unsigned int **,struct __Pb__Ui__Pe___Type );
static struct __Pb__Ui__Pe___Type __Pb__Ui__Pe___Type_Ret_create_struct___Pb__Ui__Pe___Arg_UL_Arg(unsigned int *,unsigned long long );

static struct __Pb__Ui__Pe___Type __Pb__Ui__Pe___Type_Ret_create_struct___Pb__Ui__Pe___Arg_UL_Arg(unsigned int *input1,unsigned long long input2)
{
  struct __Pb__Ui__Pe___Type output;
  output.__Pb__Ui__Pe___Type::ptr = input1;
  output.__Pb__Ui__Pe___Type::addr = input2;
  return output;
}

static struct __Pb__Ui__Pe___Type __Pb__Ui__Pe___Type_Ret_assign_and_copy___Pb____Pb__Ui__Pe____Pe___Arg___Pb__Ui__Pe___Type_Arg(unsigned int **input1,struct __Pb__Ui__Pe___Type input2)
{
   *input1 = input2.__Pb__Ui__Pe___Type::ptr;
  v_Ret_create_entry_UL_Arg_UL_Arg(((unsigned long long )input1),((unsigned long long )input2.__Pb__Ui__Pe___Type::addr));
  return __Pb__Ui__Pe___Type_Ret_create_struct___Pb__Ui__Pe___Arg_UL_Arg(input2.__Pb__Ui__Pe___Type::ptr,((unsigned long long )input1));
}
static struct __Pb__Ui__Pe___Type __Pb__Ui__Pe___Type_Ret_assign_and_copy___Pb____Pb__Ui__Pe____Pe___Arg___Pb__Ui__Pe___Arg_UL_Arg(unsigned int **,unsigned int *,unsigned long long );

static struct __Pb__Ui__Pe___Type __Pb__Ui__Pe___Type_Ret_assign_and_copy___Pb____Pb__Ui__Pe____Pe___Arg___Pb__Ui__Pe___Arg_UL_Arg(unsigned int **input1,unsigned int *input2,unsigned long long input3)
{
   *input1 = input2;
  v_Ret_create_entry_UL_Arg_UL_Arg(((unsigned long long )input1),input3);
  return __Pb__Ui__Pe___Type_Ret_create_struct___Pb__Ui__Pe___Arg_UL_Arg(input2,((unsigned long long )input1));
}
void v_Ret_execAtFirst();
void v_Ret_execAtLast();

int main()
{
  v_Ret_execAtFirst();
  unsigned int *ptr;
  __Pb__Ui__Pe___Type_Ret_assign_and_copy___Pb____Pb__Ui__Pe____Pe___Arg___Pb__Ui__Pe___Type_Arg(&ptr,__Pb__Ui__Pe___Type_Ret_Cast___Pb__v__Pe___Type_Arg(__Pb__v__Pe___Type_Ret_malloc_overload_Ul_Arg(((unsigned long )400UL))));
  unsigned int *ptr2;
  __Pb__Ui__Pe___Type_Ret_assign_and_copy___Pb____Pb__Ui__Pe____Pe___Arg___Pb__Ui__Pe___Type_Arg(&ptr2,__Pb__Ui__Pe___Type_Ret_Cast___Pb__v__Pe___Type_Arg(__Pb__v__Pe___Type_Ret_malloc_overload_Ul_Arg(((unsigned long )40UL))));
  unsigned int *start_ptr;
  __Pb__Ui__Pe___Type_Ret_assign_and_copy___Pb____Pb__Ui__Pe____Pe___Arg___Pb__Ui__Pe___Arg_UL_Arg(&start_ptr,((unsigned int *)ptr),((unsigned long long )(&ptr)));
  unsigned int *start_ptr2;
  __Pb__Ui__Pe___Type_Ret_assign_and_copy___Pb____Pb__Ui__Pe____Pe___Arg___Pb__Ui__Pe___Arg_UL_Arg(&start_ptr2,((unsigned int *)ptr2),((unsigned long long )(&ptr2)));
  unsigned int *start_ptr3;
  __Pb__Ui__Pe___Type_Ret_assign_and_copy___Pb____Pb__Ui__Pe____Pe___Arg___Pb__Ui__Pe___Type_Arg(&start_ptr3,__Pb__Ui__Pe___Type_Ret_Cast___Pb__v__Pe___Type_Arg(__Pb__v__Pe___Type_Ret_malloc_overload_Ul_Arg(((unsigned long )400UL))));
  unsigned int *start_ptr4;
  __Pb__Ui__Pe___Type_Ret_assign_and_copy___Pb____Pb__Ui__Pe____Pe___Arg___Pb__Ui__Pe___Arg_UL_Arg(&start_ptr4,((unsigned int *)start_ptr2),((unsigned long long )(&start_ptr2)));
    #if 0
    #if 1
    #endif
// CIRM Review Code
// ------
// Passing pointers to function
// ------
// Complex expressions
// Add more
    #endif
  v_Ret_execAtLast();
  return 1;
}
