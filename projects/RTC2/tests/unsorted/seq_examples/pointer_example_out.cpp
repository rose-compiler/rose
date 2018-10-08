#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#define PTR_SIZE 100
#define PTR2_SIZE 10
#define PTR3_SIZE 10
#define OUT_OF_BOUNDS_ACCESS 1

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
static int global_var0_17_2 = 0;
static struct __Pb__i__Pe___Type __Pb__i__Pe___Type_Ret_Cast___Pb__i__Pe___Arg_UL_Arg_Ul_Arg_UL_Arg(int *,unsigned long long ,unsigned long ,unsigned long long );
void v_Ret_create_entry_UL_Arg_UL_Arg_Ul_Arg_UL_Arg(unsigned long long ,unsigned long long ,unsigned long ,unsigned long long );

static struct __Pb__i__Pe___Type __Pb__i__Pe___Type_Ret_Cast___Pb__i__Pe___Arg_UL_Arg_Ul_Arg_UL_Arg(int *input1,unsigned long long input2,unsigned long input3,unsigned long long input4)
{
  v_Ret_create_entry_UL_Arg_UL_Arg_Ul_Arg_UL_Arg(input2,((unsigned long long )input1),input3,input4);
  return __Pb__i__Pe___Type_Ret_create_struct___Pb__i__Pe___Arg_UL_Arg(input1,input2);
}
static int i_Ret_NotEqual___Pb__i__Pe___Arg___Pb__i__Pe___Type_Arg(int *,struct __Pb__i__Pe___Type );

static int i_Ret_NotEqual___Pb__i__Pe___Arg___Pb__i__Pe___Type_Arg(int *input1,struct __Pb__i__Pe___Type input2)
{
  return input1 != input2.__Pb__i__Pe___Type::ptr;
}
static struct __Pb__i__Pe___Type __Pb__i__Pe___Type_Ret_assign_and_copy___Pb____Pb__i__Pe____Pe___Arg___Pb__i__Pe___Arg_UL_Arg(int **,int *,unsigned long long );

static struct __Pb__i__Pe___Type __Pb__i__Pe___Type_Ret_assign_and_copy___Pb____Pb__i__Pe____Pe___Arg___Pb__i__Pe___Arg_UL_Arg(int **input1,int *input2,unsigned long long input3)
{
   *input1 = input2;
  v_Ret_create_entry_UL_Arg_UL_Arg(((unsigned long long )input1),input3);
  return __Pb__i__Pe___Type_Ret_create_struct___Pb__i__Pe___Arg_UL_Arg(input2,((unsigned long long )input1));
}
static int *__Pb__i__Pe___Ret_deref_check___Pb__i__Pe___Arg_UL_Arg(int *,unsigned long long );
void v_Ret_check_entry_UL_Arg_UL_Arg(unsigned long long ,unsigned long long );
void v_Ret_null_check_UL_Arg(unsigned long long );

static int *__Pb__i__Pe___Ret_deref_check___Pb__i__Pe___Arg_UL_Arg(int *input1,unsigned long long input2)
{
  v_Ret_check_entry_UL_Arg_UL_Arg(((unsigned long long )input1),input2);
  v_Ret_null_check_UL_Arg(((unsigned long long )input1));
  return input1;
}
static struct __Pb__i__Pe___Type __Pb__i__Pe___Type_Ret_Increment___Pb____Pb__i__Pe____Pe___Arg(int **);

static struct __Pb__i__Pe___Type __Pb__i__Pe___Type_Ret_Increment___Pb____Pb__i__Pe____Pe___Arg(int **input1)
{
  ++( *input1);
  struct __Pb__i__Pe___Type output;
  output.__Pb__i__Pe___Type::ptr =  *input1;
  output.__Pb__i__Pe___Type::addr = ((unsigned long long )input1);
  return output;
}
static int global_var2_70_2 = 0;
static int global_var4_72_9 = 0;

struct __Pb__FILE__sFILE__typedef_declaration__Pe___Type 
{
  FILE *ptr;
  unsigned long long addr;
}
;
static struct __Pb__FILE__sFILE__typedef_declaration__Pe___Type L1186R_Ret_Cast___Pb__FILE__sFILE__typedef_declaration__Pe___Arg_UL_Arg_Ul_Arg_UL_Arg(FILE *,unsigned long long ,unsigned long ,unsigned long long );
static struct __Pb__FILE__sFILE__typedef_declaration__Pe___Type L1186R_Ret_create_struct___Pb__FILE__sFILE__typedef_declaration__Pe___Arg_UL_Arg(FILE *,unsigned long long );

static struct __Pb__FILE__sFILE__typedef_declaration__Pe___Type L1186R_Ret_create_struct___Pb__FILE__sFILE__typedef_declaration__Pe___Arg_UL_Arg(FILE *input1,unsigned long long input2)
{
  struct __Pb__FILE__sFILE__typedef_declaration__Pe___Type output;
  output.__Pb__FILE__sFILE__typedef_declaration__Pe___Type::ptr = input1;
  output.__Pb__FILE__sFILE__typedef_declaration__Pe___Type::addr = input2;
  return output;
}

static struct __Pb__FILE__sFILE__typedef_declaration__Pe___Type L1186R_Ret_Cast___Pb__FILE__sFILE__typedef_declaration__Pe___Arg_UL_Arg_Ul_Arg_UL_Arg(FILE *input1,unsigned long long input2,unsigned long input3,unsigned long long input4)
{
  v_Ret_create_entry_UL_Arg_UL_Arg_Ul_Arg_UL_Arg(input2,((unsigned long long )input1),input3,input4);
  return L1186R_Ret_create_struct___Pb__FILE__sFILE__typedef_declaration__Pe___Arg_UL_Arg(input1,input2);
}
static FILE *__Pb__FILE__sFILE__typedef_declaration__Pe___Ret_return_pointer_L1186R_Arg(struct __Pb__FILE__sFILE__typedef_declaration__Pe___Type );

static FILE *__Pb__FILE__sFILE__typedef_declaration__Pe___Ret_return_pointer_L1186R_Arg(struct __Pb__FILE__sFILE__typedef_declaration__Pe___Type input1)
{
  return input1.__Pb__FILE__sFILE__typedef_declaration__Pe___Type::ptr;
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
  int *ptr;
  __Pb__i__Pe___Type_Ret_assign_and_copy___Pb____Pb__i__Pe____Pe___Arg___Pb__i__Pe___Type_Arg(&ptr,__Pb__i__Pe___Type_Ret_Cast___Pb__v__Pe___Type_Arg(__Pb__v__Pe___Type_Ret_malloc_overload_Ul_Arg(((unsigned long )400UL))));
  int *ptr2;
  __Pb__i__Pe___Type_Ret_assign_and_copy___Pb____Pb__i__Pe____Pe___Arg___Pb__i__Pe___Type_Arg(&ptr2,__Pb__i__Pe___Type_Ret_Cast___Pb__v__Pe___Type_Arg(__Pb__v__Pe___Type_Ret_malloc_overload_Ul_Arg(((unsigned long )40UL))));
  (__builtin_expect((!i_Ret_NotEqual___Pb__i__Pe___Arg___Pb__i__Pe___Type_Arg(ptr2,__Pb__i__Pe___Type_Ret_Cast___Pb__i__Pe___Arg_UL_Arg_Ul_Arg_UL_Arg(((int *)global_var0_17_2),((unsigned long long )(&global_var0_17_2)),((unsigned long )(sizeof(global_var0_17_2))),((unsigned long long )50)))),0L))?__assert_rtn(__func__,"/Users/vanka1/research/compilers/rose/rose_build/projects/RTC/pointer_example.cpp",17,"ptr2 != NULL") : ((void )0);
  int *start_ptr;
  __Pb__i__Pe___Type_Ret_assign_and_copy___Pb____Pb__i__Pe____Pe___Arg___Pb__i__Pe___Arg_UL_Arg(&start_ptr,((int *)ptr),((unsigned long long )(&ptr)));
  int *start_ptr2;
  __Pb__i__Pe___Type_Ret_assign_and_copy___Pb____Pb__i__Pe____Pe___Arg___Pb__i__Pe___Arg_UL_Arg(&start_ptr2,((int *)ptr2),((unsigned long long )(&ptr2)));
// Crossing the boundary of ptr. The condition should
// be less than, not less than or equal to
// ptr[PTR_SIZE] is an out-of-bounds access
//    for(int index = 0; index <= (PTR_SIZE + OUT_OF_BOUNDS_ACCESS); index++) {
  for (int index = 0; index < 100; index++) {
     *__Pb__i__Pe___Ret_deref_check___Pb__i__Pe___Arg_UL_Arg(ptr,((unsigned long long )(&ptr))) = index;
    __Pb__i__Pe___Type_Ret_Increment___Pb____Pb__i__Pe____Pe___Arg(&ptr);
  }
// Resetting ptr to start_ptr, so that it points to the beginning
// of the allocation
  __Pb__i__Pe___Type_Ret_assign_and_copy___Pb____Pb__i__Pe____Pe___Arg___Pb__i__Pe___Arg_UL_Arg(&ptr,((int *)start_ptr),((unsigned long long )(&start_ptr)));
// Printing what we wrote above
//    for(int index = 0; index <= (PTR_SIZE + OUT_OF_BOUNDS_ACCESS); index++) {
  for (int index = 0; index < 100; index++) {
    printf("ptr[%d]=%d\n",index, *__Pb__i__Pe___Ret_deref_check___Pb__i__Pe___Arg_UL_Arg(ptr,((unsigned long long )(&ptr))));
    __Pb__i__Pe___Type_Ret_Increment___Pb____Pb__i__Pe____Pe___Arg(&ptr);
  }
#if 0
// Resetting ptr to start_ptr, so that it points to the beginning
// of the allocation
// Memsetting ptr and ptr2 allocations, in one go.
// This is also crossing the boundaries of ptr. It assumes that
// ptr and ptr2 are in contiguous locations
// Resetting ptr to start_ptr, so that it points to the beginning
// of the allocation
// Printing ptr and ptr2 *and* one more beyond ptr2, all using 
// ptr! This still works since malloc asks for more than it needs
// always.
#endif
  (__builtin_expect((!i_Ret_NotEqual___Pb__i__Pe___Arg___Pb__i__Pe___Type_Arg(ptr2,__Pb__i__Pe___Type_Ret_Cast___Pb__i__Pe___Arg_UL_Arg_Ul_Arg_UL_Arg(((int *)global_var2_70_2),((unsigned long long )(&global_var2_70_2)),((unsigned long )(sizeof(global_var2_70_2))),((unsigned long long )50)))),0L))?__assert_rtn(__func__,"/Users/vanka1/research/compilers/rose/rose_build/projects/RTC/pointer_example.cpp",70,"ptr2 != NULL") : ((void )0);
  printf("Before free ptr2\n");
  fflush(__Pb__FILE__sFILE__typedef_declaration__Pe___Ret_return_pointer_L1186R_Arg(L1186R_Ret_Cast___Pb__FILE__sFILE__typedef_declaration__Pe___Arg_UL_Arg_Ul_Arg_UL_Arg(((FILE *)global_var4_72_9),((unsigned long long )(&global_var4_72_9)),((unsigned long )(sizeof(global_var4_72_9))),((unsigned long long )50))));
  v_Ret_free_overload___Pb__v__Pe___Type_Arg(__Pb__v__Pe___Type_Ret_Cast___Pb__i__Pe___Arg_UL_Arg(ptr2,((unsigned long long )(&ptr2))));
  v_Ret_free_overload___Pb__v__Pe___Type_Arg(__Pb__v__Pe___Type_Ret_Cast___Pb__i__Pe___Arg_UL_Arg(start_ptr,((unsigned long long )(&start_ptr))));
#if 0
    #if 0
// Retrying the print above, after freeing ptr2. This should
// crash--- and it does!
    #endif
// Allocating another pointer
// This allocation might take the place of ptr2. In this case,
// printing ptr beyond its boundaries should be okay
// Nope this also crashes!
#endif    
  v_Ret_execAtLast();
  return 1;
}
