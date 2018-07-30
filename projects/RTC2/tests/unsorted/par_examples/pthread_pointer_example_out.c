#include <pthread.h>
//#include <cstdio>
//#include <cstdlib>
//#include <queue>
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#define NUM_THREADS 3
#define COUNT 100
//typedef std::queue<int> IntQ;
// elements are "push"ed into the back, 
// and "pop"ped from the front
//IntQ SendQ;
typedef enum __unnamed_enum___F0_L18_C9_FALSE__COMMA__TRUE {FALSE,TRUE}BOOLEAN;
BOOLEAN finish;
pthread_mutex_t lock;
unsigned int *array1;
unsigned int *array2;
// Node in the queue
typedef struct node {
struct node *next;
int val;}node;
// Queue implementation
//typedef struct QueueImpl {
struct node *front_node;
struct node *back_node;
// pop from the front
static int global_var0_39_19 = 0;

struct __Pb__node__Pe___Type 
{
  struct node *ptr;
  unsigned long long addr;
}
;
static struct __Pb__node__Pe___Type __Pb__node__Pe___Type_Ret_Cast___Pb__node__Pe___Arg_UL_Arg_Ul_Arg_UL_Arg(struct node *,unsigned long long ,unsigned long ,unsigned long long );
void v_Ret_create_entry_UL_Arg_UL_Arg_Ul_Arg_UL_Arg(unsigned long long ,unsigned long long ,unsigned long ,unsigned long long );
static struct __Pb__node__Pe___Type __Pb__node__Pe___Type_Ret_create_struct___Pb__node__Pe___Arg_UL_Arg(struct node *,unsigned long long );

static struct __Pb__node__Pe___Type __Pb__node__Pe___Type_Ret_create_struct___Pb__node__Pe___Arg_UL_Arg(struct node *input1,unsigned long long input2)
{
  struct __Pb__node__Pe___Type output;
  output.ptr = input1;
  output.addr = input2;
  return output;
}

static struct __Pb__node__Pe___Type __Pb__node__Pe___Type_Ret_Cast___Pb__node__Pe___Arg_UL_Arg_Ul_Arg_UL_Arg(struct node *input1,unsigned long long input2,unsigned long input3,unsigned long long input4)
{
  v_Ret_create_entry_UL_Arg_UL_Arg_Ul_Arg_UL_Arg(input2,((unsigned long long )input1),input3,input4);
  return __Pb__node__Pe___Type_Ret_create_struct___Pb__node__Pe___Arg_UL_Arg(input1,input2);
}
static int i_Ret_Equality___Pb__node__Pe___Arg___Pb__node__Pe___Type_Arg(struct node *,struct __Pb__node__Pe___Type );

static int i_Ret_Equality___Pb__node__Pe___Arg___Pb__node__Pe___Type_Arg(struct node *input1,struct __Pb__node__Pe___Type input2)
{
  return input1 == input2.ptr;
}
static char* global_var2_40_10 = "empty list. Nothing to be popped. Quitting\n";

struct __Pb__Cc__Pe___Type 
{
  const char *ptr;
  unsigned long long addr;
}
;
static struct __Pb__Cc__Pe___Type __Pb__Cc__Pe___Type_Ret_Cast___Pb__Cc__Pe___Arg_UL_Arg_Ul_Arg_UL_Arg(const char *,unsigned long long ,unsigned long ,unsigned long long );
static struct __Pb__Cc__Pe___Type __Pb__Cc__Pe___Type_Ret_create_struct___Pb__Cc__Pe___Arg_UL_Arg(const char *,unsigned long long );

static struct __Pb__Cc__Pe___Type __Pb__Cc__Pe___Type_Ret_create_struct___Pb__Cc__Pe___Arg_UL_Arg(const char *input1,unsigned long long input2)
{
  struct __Pb__Cc__Pe___Type output;
  output.ptr = input1;
  output.addr = input2;
  return output;
}

static struct __Pb__Cc__Pe___Type __Pb__Cc__Pe___Type_Ret_Cast___Pb__Cc__Pe___Arg_UL_Arg_Ul_Arg_UL_Arg(const char *input1,unsigned long long input2,unsigned long input3,unsigned long long input4)
{
  v_Ret_create_entry_UL_Arg_UL_Arg_Ul_Arg_UL_Arg(input2,((unsigned long long )input1),input3,input4);
  return __Pb__Cc__Pe___Type_Ret_create_struct___Pb__Cc__Pe___Arg_UL_Arg(input1,input2);
}
static const char *__Pb__Cc__Pe___Ret_return_pointer___Pb__Cc__Pe___Type_Arg(struct __Pb__Cc__Pe___Type );

static const char *__Pb__Cc__Pe___Ret_return_pointer___Pb__Cc__Pe___Type_Arg(struct __Pb__Cc__Pe___Type input1)
{
  return input1.ptr;
}
static int i_Ret_Equality___Pb__node__Pe___Arg___Pb__node__Pe___Arg(struct node *,struct node *);

static int i_Ret_Equality___Pb__node__Pe___Arg___Pb__node__Pe___Arg(struct node *input1,struct node *input2)
{
  return input1 == input2;
}
static struct node *__Pb__node__Pe___Ret_deref_check___Pb__node__Pe___Arg_UL_Arg(struct node *,unsigned long long );
void v_Ret_check_entry_UL_Arg_UL_Arg(unsigned long long ,unsigned long long );
void v_Ret_null_check_UL_Arg(unsigned long long );

static struct node *__Pb__node__Pe___Ret_deref_check___Pb__node__Pe___Arg_UL_Arg(struct node *input1,unsigned long long input2)
{
  v_Ret_check_entry_UL_Arg_UL_Arg(((unsigned long long )input1),input2);
  v_Ret_null_check_UL_Arg(((unsigned long long )input1));
  return input1;
}
static int global_var4_45_3 = 0;
static char* global_var6_45_3 = "/Users/vanka1/research/compilers/rose/rose_build/projects/RTC/pthread_pointer_example.c";
static char* global_var8_45_3 = "front_node->next == NULL";

struct __Pb__v__Pe___Type 
{
  void *ptr;
  unsigned long long addr;
}
;
static struct __Pb__v__Pe___Type __Pb__v__Pe___Type_Ret_Cast___Pb__node__Pe___Arg_UL_Arg(struct node *,unsigned long long );

static struct __Pb__v__Pe___Type __Pb__v__Pe___Type_Ret_Cast___Pb__node__Pe___Arg_UL_Arg(struct node *input1,unsigned long long input2)
{
  struct __Pb__v__Pe___Type output;
  output.ptr = ((void *)input1);
  output.addr = input2;
  return output;
}
void v_Ret_free_overload___Pb__v__Pe___Type_Arg(struct __Pb__v__Pe___Type );
static int global_var10_47_16 = 0;
static struct __Pb__node__Pe___Type __Pb__node__Pe___Type_Ret_assign_and_copy___Pb____Pb__node__Pe____Pe___Arg___Pb__node__Pe___Type_Arg(struct node **,struct __Pb__node__Pe___Type );
void v_Ret_create_entry_UL_Arg_UL_Arg(unsigned long long ,unsigned long long );

static struct __Pb__node__Pe___Type __Pb__node__Pe___Type_Ret_assign_and_copy___Pb____Pb__node__Pe____Pe___Arg___Pb__node__Pe___Type_Arg(struct node **input1,struct __Pb__node__Pe___Type input2)
{
   *input1 = input2.ptr;
  v_Ret_create_entry_UL_Arg_UL_Arg(((unsigned long long )input1),((unsigned long long )input2.addr));
  return __Pb__node__Pe___Type_Ret_create_struct___Pb__node__Pe___Arg_UL_Arg(input2.ptr,((unsigned long long )input1));
}
static int global_var12_48_15 = 0;
static struct __Pb__node__Pe___Type __Pb__node__Pe___Type_Ret_assign_and_copy___Pb____Pb__node__Pe____Pe___Arg___Pb__node__Pe___Arg_UL_Arg(struct node **,struct node *,unsigned long long );

static struct __Pb__node__Pe___Type __Pb__node__Pe___Type_Ret_assign_and_copy___Pb____Pb__node__Pe____Pe___Arg___Pb__node__Pe___Arg_UL_Arg(struct node **input1,struct node *input2,unsigned long long input3)
{
   *input1 = input2;
  v_Ret_create_entry_UL_Arg_UL_Arg(((unsigned long long )input1),input3);
  return __Pb__node__Pe___Type_Ret_create_struct___Pb__node__Pe___Arg_UL_Arg(input2,((unsigned long long )input1));
}

void pop()
{
  if (i_Ret_Equality___Pb__node__Pe___Arg___Pb__node__Pe___Type_Arg(front_node,__Pb__node__Pe___Type_Ret_Cast___Pb__node__Pe___Arg_UL_Arg_Ul_Arg_UL_Arg(((struct node *)global_var0_39_19),((unsigned long long )(&global_var0_39_19)),((unsigned long )(sizeof(global_var0_39_19))),((unsigned long long )50)))) {
    printf(__Pb__Cc__Pe___Ret_return_pointer___Pb__Cc__Pe___Type_Arg(__Pb__Cc__Pe___Type_Ret_Cast___Pb__Cc__Pe___Arg_UL_Arg_Ul_Arg_UL_Arg(((const char *)global_var2_40_10),((unsigned long long )(&global_var2_40_10)),((unsigned long )(sizeof(global_var2_40_10))),((unsigned long long )50))));
    exit(1);
  }
  else if (i_Ret_Equality___Pb__node__Pe___Arg___Pb__node__Pe___Arg(front_node,back_node)) {
// single node in the list
    (__builtin_expect((!i_Ret_Equality___Pb__node__Pe___Arg___Pb__node__Pe___Type_Arg(__Pb__node__Pe___Ret_deref_check___Pb__node__Pe___Arg_UL_Arg(front_node,((unsigned long long )(&front_node))) -> next,__Pb__node__Pe___Type_Ret_Cast___Pb__node__Pe___Arg_UL_Arg_Ul_Arg_UL_Arg(((struct node *)global_var4_45_3),((unsigned long long )(&global_var4_45_3)),((unsigned long )(sizeof(global_var4_45_3))),((unsigned long long )50)))),0L) != 0L)?__assert_rtn(__func__,__Pb__Cc__Pe___Ret_return_pointer___Pb__Cc__Pe___Type_Arg(__Pb__Cc__Pe___Type_Ret_Cast___Pb__Cc__Pe___Arg_UL_Arg_Ul_Arg_UL_Arg(((const char *)global_var6_45_3),((unsigned long long )(&global_var6_45_3)),((unsigned long )(sizeof(global_var6_45_3))),((unsigned long long )50))),45,__Pb__Cc__Pe___Ret_return_pointer___Pb__Cc__Pe___Type_Arg(__Pb__Cc__Pe___Type_Ret_Cast___Pb__Cc__Pe___Arg_UL_Arg_Ul_Arg_UL_Arg(((const char *)global_var8_45_3),((unsigned long long )(&global_var8_45_3)),((unsigned long )(sizeof(global_var8_45_3))),((unsigned long long )50)))) : ((void )0);
    v_Ret_free_overload___Pb__v__Pe___Type_Arg(__Pb__v__Pe___Type_Ret_Cast___Pb__node__Pe___Arg_UL_Arg(front_node,((unsigned long long )(&front_node))));
    __Pb__node__Pe___Type_Ret_assign_and_copy___Pb____Pb__node__Pe____Pe___Arg___Pb__node__Pe___Type_Arg(&front_node,__Pb__node__Pe___Type_Ret_Cast___Pb__node__Pe___Arg_UL_Arg_Ul_Arg_UL_Arg(((struct node *)global_var10_47_16),((unsigned long long )(&global_var10_47_16)),((unsigned long )(sizeof(global_var10_47_16))),((unsigned long long )50)));
    __Pb__node__Pe___Type_Ret_assign_and_copy___Pb____Pb__node__Pe____Pe___Arg___Pb__node__Pe___Type_Arg(&back_node,__Pb__node__Pe___Type_Ret_Cast___Pb__node__Pe___Arg_UL_Arg_Ul_Arg_UL_Arg(((struct node *)global_var12_48_15),((unsigned long long )(&global_var12_48_15)),((unsigned long )(sizeof(global_var12_48_15))),((unsigned long long )50)));
  }
  else {
    struct node *popped;
    __Pb__node__Pe___Type_Ret_assign_and_copy___Pb____Pb__node__Pe____Pe___Arg___Pb__node__Pe___Arg_UL_Arg(&popped,((struct node *)front_node),((unsigned long long )(&front_node)));
    __Pb__node__Pe___Type_Ret_assign_and_copy___Pb____Pb__node__Pe____Pe___Arg___Pb__node__Pe___Arg_UL_Arg(&front_node,((struct node *)(__Pb__node__Pe___Ret_deref_check___Pb__node__Pe___Arg_UL_Arg(front_node,((unsigned long long )(&front_node))) -> next)),((unsigned long long )(&__Pb__node__Pe___Ret_deref_check___Pb__node__Pe___Arg_UL_Arg(front_node,((unsigned long long )(&front_node))) -> next)));
    v_Ret_free_overload___Pb__v__Pe___Type_Arg(__Pb__v__Pe___Type_Ret_Cast___Pb__node__Pe___Arg_UL_Arg(popped,((unsigned long long )(&popped))));
  }
}
// push at the back
static int global_var14_59_18 = 0;
struct __Pb__v__Pe___Type __Pb__v__Pe___Type_Ret_malloc_overload_Ul_Arg(unsigned long );
static struct __Pb__node__Pe___Type __Pb__node__Pe___Type_Ret_Cast___Pb__v__Pe___Type_Arg(struct __Pb__v__Pe___Type );

static struct __Pb__node__Pe___Type __Pb__node__Pe___Type_Ret_Cast___Pb__v__Pe___Type_Arg(struct __Pb__v__Pe___Type input1)
{
  struct __Pb__node__Pe___Type output;
  output.ptr = ((struct node *)input1.ptr);
  output.addr = input1.addr;
  return output;
}
static int global_var16_62_21 = 0;
static int global_var18_65_3 = 0;
static char* global_var20_65_3 = "/Users/vanka1/research/compilers/rose/rose_build/projects/RTC/pthread_pointer_example.c";
static char* global_var22_65_3 = "front_node == NULL";
static int global_var24_70_20 = 0;
static int global_var26_73_3 = 0;
static char* global_var28_73_3 = "/Users/vanka1/research/compilers/rose/rose_build/projects/RTC/pthread_pointer_example.c";
static char* global_var30_73_3 = "back_node->next == NULL";

void push(int val)
{
  if (i_Ret_Equality___Pb__node__Pe___Arg___Pb__node__Pe___Type_Arg(back_node,__Pb__node__Pe___Type_Ret_Cast___Pb__node__Pe___Arg_UL_Arg_Ul_Arg_UL_Arg(((struct node *)global_var14_59_18),((unsigned long long )(&global_var14_59_18)),((unsigned long )(sizeof(global_var14_59_18))),((unsigned long long )50)))) {
// empty list
    __Pb__node__Pe___Type_Ret_assign_and_copy___Pb____Pb__node__Pe____Pe___Arg___Pb__node__Pe___Type_Arg(&back_node,__Pb__node__Pe___Type_Ret_Cast___Pb__v__Pe___Type_Arg(__Pb__v__Pe___Type_Ret_malloc_overload_Ul_Arg(((unsigned long )16UL))));
    __Pb__node__Pe___Type_Ret_assign_and_copy___Pb____Pb__node__Pe____Pe___Arg___Pb__node__Pe___Type_Arg(&__Pb__node__Pe___Ret_deref_check___Pb__node__Pe___Arg_UL_Arg(back_node,((unsigned long long )(&back_node))) -> next,__Pb__node__Pe___Type_Ret_Cast___Pb__node__Pe___Arg_UL_Arg_Ul_Arg_UL_Arg(((struct node *)global_var16_62_21),((unsigned long long )(&global_var16_62_21)),((unsigned long )(sizeof(global_var16_62_21))),((unsigned long long )50)));
    __Pb__node__Pe___Ret_deref_check___Pb__node__Pe___Arg_UL_Arg(back_node,((unsigned long long )(&back_node))) -> val = val;
// make the front node point to the same node
    (__builtin_expect((!i_Ret_Equality___Pb__node__Pe___Arg___Pb__node__Pe___Type_Arg(front_node,__Pb__node__Pe___Type_Ret_Cast___Pb__node__Pe___Arg_UL_Arg_Ul_Arg_UL_Arg(((struct node *)global_var18_65_3),((unsigned long long )(&global_var18_65_3)),((unsigned long )(sizeof(global_var18_65_3))),((unsigned long long )50)))),0L) != 0L)?__assert_rtn(__func__,__Pb__Cc__Pe___Ret_return_pointer___Pb__Cc__Pe___Type_Arg(__Pb__Cc__Pe___Type_Ret_Cast___Pb__Cc__Pe___Arg_UL_Arg_Ul_Arg_UL_Arg(((const char *)global_var20_65_3),((unsigned long long )(&global_var20_65_3)),((unsigned long )(sizeof(global_var20_65_3))),((unsigned long long )50))),65,__Pb__Cc__Pe___Ret_return_pointer___Pb__Cc__Pe___Type_Arg(__Pb__Cc__Pe___Type_Ret_Cast___Pb__Cc__Pe___Arg_UL_Arg_Ul_Arg_UL_Arg(((const char *)global_var22_65_3),((unsigned long long )(&global_var22_65_3)),((unsigned long )(sizeof(global_var22_65_3))),((unsigned long long )50)))) : ((void )0);
    __Pb__node__Pe___Type_Ret_assign_and_copy___Pb____Pb__node__Pe____Pe___Arg___Pb__node__Pe___Arg_UL_Arg(&front_node,((struct node *)back_node),((unsigned long long )(&back_node)));
  }
  else {
    struct node *new_node;
    __Pb__node__Pe___Type_Ret_assign_and_copy___Pb____Pb__node__Pe____Pe___Arg___Pb__node__Pe___Type_Arg(&new_node,__Pb__node__Pe___Type_Ret_Cast___Pb__v__Pe___Type_Arg(__Pb__v__Pe___Type_Ret_malloc_overload_Ul_Arg(((unsigned long )16UL))));
    __Pb__node__Pe___Type_Ret_assign_and_copy___Pb____Pb__node__Pe____Pe___Arg___Pb__node__Pe___Type_Arg(&__Pb__node__Pe___Ret_deref_check___Pb__node__Pe___Arg_UL_Arg(new_node,((unsigned long long )(&new_node))) -> next,__Pb__node__Pe___Type_Ret_Cast___Pb__node__Pe___Arg_UL_Arg_Ul_Arg_UL_Arg(((struct node *)global_var24_70_20),((unsigned long long )(&global_var24_70_20)),((unsigned long )(sizeof(global_var24_70_20))),((unsigned long long )50)));
    __Pb__node__Pe___Ret_deref_check___Pb__node__Pe___Arg_UL_Arg(new_node,((unsigned long long )(&new_node))) -> val = val;
// make back_node->next point to the new node
    (__builtin_expect((!i_Ret_Equality___Pb__node__Pe___Arg___Pb__node__Pe___Type_Arg(__Pb__node__Pe___Ret_deref_check___Pb__node__Pe___Arg_UL_Arg(back_node,((unsigned long long )(&back_node))) -> next,__Pb__node__Pe___Type_Ret_Cast___Pb__node__Pe___Arg_UL_Arg_Ul_Arg_UL_Arg(((struct node *)global_var26_73_3),((unsigned long long )(&global_var26_73_3)),((unsigned long )(sizeof(global_var26_73_3))),((unsigned long long )50)))),0L) != 0L)?__assert_rtn(__func__,__Pb__Cc__Pe___Ret_return_pointer___Pb__Cc__Pe___Type_Arg(__Pb__Cc__Pe___Type_Ret_Cast___Pb__Cc__Pe___Arg_UL_Arg_Ul_Arg_UL_Arg(((const char *)global_var28_73_3),((unsigned long long )(&global_var28_73_3)),((unsigned long )(sizeof(global_var28_73_3))),((unsigned long long )50))),73,__Pb__Cc__Pe___Ret_return_pointer___Pb__Cc__Pe___Type_Arg(__Pb__Cc__Pe___Type_Ret_Cast___Pb__Cc__Pe___Arg_UL_Arg_Ul_Arg_UL_Arg(((const char *)global_var30_73_3),((unsigned long long )(&global_var30_73_3)),((unsigned long )(sizeof(global_var30_73_3))),((unsigned long long )50)))) : ((void )0);
    __Pb__node__Pe___Type_Ret_assign_and_copy___Pb____Pb__node__Pe____Pe___Arg___Pb__node__Pe___Arg_UL_Arg(&__Pb__node__Pe___Ret_deref_check___Pb__node__Pe___Arg_UL_Arg(back_node,((unsigned long long )(&back_node))) -> next,((struct node *)new_node),((unsigned long long )(&new_node)));
// then back_node itself points to the new node
    __Pb__node__Pe___Type_Ret_assign_and_copy___Pb____Pb__node__Pe____Pe___Arg___Pb__node__Pe___Arg_UL_Arg(&back_node,((struct node *)new_node),((unsigned long long )(&new_node)));
  }
}

struct __Pb__node__Pe___Type front()
{
  return __Pb__node__Pe___Type_Ret_create_struct___Pb__node__Pe___Arg_UL_Arg(front_node,((unsigned long long )(&front_node)));
}

struct __Pb__node__Pe___Type back()
{
  return __Pb__node__Pe___Type_Ret_create_struct___Pb__node__Pe___Arg_UL_Arg(back_node,((unsigned long long )(&back_node)));
}
// call me first
static int global_var32_92_15 = 0;
static int global_var34_93_14 = 0;

void init()
{
  __Pb__node__Pe___Type_Ret_assign_and_copy___Pb____Pb__node__Pe____Pe___Arg___Pb__node__Pe___Type_Arg(&front_node,__Pb__node__Pe___Type_Ret_Cast___Pb__node__Pe___Arg_UL_Arg_Ul_Arg_UL_Arg(((struct node *)global_var32_92_15),((unsigned long long )(&global_var32_92_15)),((unsigned long )(sizeof(global_var32_92_15))),((unsigned long long )50)));
  __Pb__node__Pe___Type_Ret_assign_and_copy___Pb____Pb__node__Pe____Pe___Arg___Pb__node__Pe___Type_Arg(&back_node,__Pb__node__Pe___Type_Ret_Cast___Pb__node__Pe___Arg_UL_Arg_Ul_Arg_UL_Arg(((struct node *)global_var34_93_14),((unsigned long long )(&global_var34_93_14)),((unsigned long )(sizeof(global_var34_93_14))),((unsigned long long )50)));
}
// check if empty
static int global_var36_98_19 = 0;
static int global_var38_99_3 = 0;
static char* global_var40_99_3 = "/Users/vanka1/research/compilers/rose/rose_build/projects/RTC/pthread_pointer_example.c";
static char* global_var42_99_3 = "back_node == NULL";

int empty()
{
  if (i_Ret_Equality___Pb__node__Pe___Arg___Pb__node__Pe___Type_Arg(front_node,__Pb__node__Pe___Type_Ret_Cast___Pb__node__Pe___Arg_UL_Arg_Ul_Arg_UL_Arg(((struct node *)global_var36_98_19),((unsigned long long )(&global_var36_98_19)),((unsigned long )(sizeof(global_var36_98_19))),((unsigned long long )50)))) {
    (__builtin_expect((!i_Ret_Equality___Pb__node__Pe___Arg___Pb__node__Pe___Type_Arg(back_node,__Pb__node__Pe___Type_Ret_Cast___Pb__node__Pe___Arg_UL_Arg_Ul_Arg_UL_Arg(((struct node *)global_var38_99_3),((unsigned long long )(&global_var38_99_3)),((unsigned long )(sizeof(global_var38_99_3))),((unsigned long long )50)))),0L) != 0L)?__assert_rtn(__func__,__Pb__Cc__Pe___Ret_return_pointer___Pb__Cc__Pe___Type_Arg(__Pb__Cc__Pe___Type_Ret_Cast___Pb__Cc__Pe___Arg_UL_Arg_Ul_Arg_UL_Arg(((const char *)global_var40_99_3),((unsigned long long )(&global_var40_99_3)),((unsigned long )(sizeof(global_var40_99_3))),((unsigned long long )50))),99,__Pb__Cc__Pe___Ret_return_pointer___Pb__Cc__Pe___Type_Arg(__Pb__Cc__Pe___Type_Ret_Cast___Pb__Cc__Pe___Arg_UL_Arg_Ul_Arg_UL_Arg(((const char *)global_var42_99_3),((unsigned long long )(&global_var42_99_3)),((unsigned long )(sizeof(global_var42_99_3))),((unsigned long long )50)))) : ((void )0);
    return 1;
  }
  else {
    return 0;
  }
}
// print queue
static char* global_var44_110_10 = "Queue is empty, nothing to print\n";
static char* global_var46_113_10 = "Printing Q\n";
static char* global_var48_117_12 = "%d,";
static int global_var50_119_18 = 0;
static int i_Ret_NotEqual___Pb__node__Pe___Arg___Pb__node__Pe___Type_Arg(struct node *,struct __Pb__node__Pe___Type );

static int i_Ret_NotEqual___Pb__node__Pe___Arg___Pb__node__Pe___Type_Arg(struct node *input1,struct __Pb__node__Pe___Type input2)
{
  return input1 != input2.ptr;
}

void printQ()
{
  if (empty() != 0) {
    printf(__Pb__Cc__Pe___Ret_return_pointer___Pb__Cc__Pe___Type_Arg(__Pb__Cc__Pe___Type_Ret_Cast___Pb__Cc__Pe___Arg_UL_Arg_Ul_Arg_UL_Arg(((const char *)global_var44_110_10),((unsigned long long )(&global_var44_110_10)),((unsigned long )(sizeof(global_var44_110_10))),((unsigned long long )50))));
  }
  else {
    printf(__Pb__Cc__Pe___Ret_return_pointer___Pb__Cc__Pe___Type_Arg(__Pb__Cc__Pe___Type_Ret_Cast___Pb__Cc__Pe___Arg_UL_Arg_Ul_Arg_UL_Arg(((const char *)global_var46_113_10),((unsigned long long )(&global_var46_113_10)),((unsigned long )(sizeof(global_var46_113_10))),((unsigned long long )50))));
// go from front to back
    struct node *temp;
    __Pb__node__Pe___Type_Ret_assign_and_copy___Pb____Pb__node__Pe____Pe___Arg___Pb__node__Pe___Arg_UL_Arg(&temp,((struct node *)front_node),((unsigned long long )(&front_node)));
    do {
      printf(__Pb__Cc__Pe___Ret_return_pointer___Pb__Cc__Pe___Type_Arg(__Pb__Cc__Pe___Type_Ret_Cast___Pb__Cc__Pe___Arg_UL_Arg_Ul_Arg_UL_Arg(((const char *)global_var48_117_12),((unsigned long long )(&global_var48_117_12)),((unsigned long )(sizeof(global_var48_117_12))),((unsigned long long )50))),__Pb__node__Pe___Ret_deref_check___Pb__node__Pe___Arg_UL_Arg(temp,((unsigned long long )(&temp))) -> val);
      __Pb__node__Pe___Type_Ret_assign_and_copy___Pb____Pb__node__Pe____Pe___Arg___Pb__node__Pe___Arg_UL_Arg(&temp,((struct node *)(__Pb__node__Pe___Ret_deref_check___Pb__node__Pe___Arg_UL_Arg(temp,((unsigned long long )(&temp))) -> next)),((unsigned long long )(&__Pb__node__Pe___Ret_deref_check___Pb__node__Pe___Arg_UL_Arg(temp,((unsigned long long )(&temp))) -> next)));
    }while (i_Ret_NotEqual___Pb__node__Pe___Arg___Pb__node__Pe___Type_Arg(temp,__Pb__node__Pe___Type_Ret_Cast___Pb__node__Pe___Arg_UL_Arg_Ul_Arg_UL_Arg(((struct node *)global_var50_119_18),((unsigned long long )(&global_var50_119_18)),((unsigned long )(sizeof(global_var50_119_18))),((unsigned long long )50))));
  }
}
//} Queue;
//Queue SendQ;
#if 0
//    while(!finish || !SendQ.empty()) {
//        if(!SendQ.empty()) {
//            int val = SendQ.front();
//            SendQ.pop();
//        SendQ.push(tid);
#endif
static char* global_var52_171_9 = "%s\n";

struct __Pb__c__Pe___Type 
{
  char *ptr;
  unsigned long long addr;
}
;
static struct __Pb__c__Pe___Type __Pb__c__Pe___Type_Ret_Cast___Pb__v__Pe___Type_Arg(struct __Pb__v__Pe___Type );

static struct __Pb__c__Pe___Type __Pb__c__Pe___Type_Ret_Cast___Pb__v__Pe___Type_Arg(struct __Pb__v__Pe___Type input1)
{
  struct __Pb__c__Pe___Type output;
  output.ptr = ((char *)input1.ptr);
  output.addr = input1.addr;
  return output;
}
static char *__Pb__c__Pe___Ret_return_pointer___Pb__c__Pe___Type_Arg(struct __Pb__c__Pe___Type );

static char *__Pb__c__Pe___Ret_return_pointer___Pb__c__Pe___Type_Arg(struct __Pb__c__Pe___Type input1)
{
  return input1.ptr;
}
void v_Ret_array_bound_check_using_lookup_UL_Arg_Ul_Arg(unsigned long long ,unsigned long );
static char* global_var60_183_10 = "array1[%d] = %d\n";
static char* global_var63_186_9 = "freeing array1\n";
static struct __Pb__v__Pe___Type __Pb__v__Pe___Type_Ret_Cast___Pb__Ui__Pe___Arg_UL_Arg(unsigned int *,unsigned long long );

static struct __Pb__v__Pe___Type __Pb__v__Pe___Type_Ret_Cast___Pb__Ui__Pe___Arg_UL_Arg(unsigned int *input1,unsigned long long input2)
{
  struct __Pb__v__Pe___Type output;
  output.ptr = ((void *)input1);
  output.addr = input2;
  return output;
}

struct __Pb__v__Pe___Type Fibonacci(struct __Pb__v__Pe___Type threadid_str)
{
  (v_Ret_create_entry_UL_Arg_UL_Arg(((unsigned long long )(&threadid_str.ptr)),threadid_str.addr) , threadid_str.addr = ((unsigned long long )(&threadid_str.ptr)));
  printf(__Pb__Cc__Pe___Ret_return_pointer___Pb__Cc__Pe___Type_Arg(__Pb__Cc__Pe___Type_Ret_Cast___Pb__Cc__Pe___Arg_UL_Arg_Ul_Arg_UL_Arg(((const char *)global_var52_171_9),((unsigned long long )(&global_var52_171_9)),((unsigned long )(sizeof(global_var52_171_9))),((unsigned long long )50))),__Pb__c__Pe___Ret_return_pointer___Pb__c__Pe___Type_Arg(__Pb__c__Pe___Type_Ret_Cast___Pb__v__Pe___Type_Arg(threadid_str)));
  for (int index = 0; index < 100; index++) {
    if (index == 0) {
      int i_index_54;
      (i_index_54 = index , v_Ret_array_bound_check_using_lookup_UL_Arg_Ul_Arg(((unsigned long long )(&array1)),((unsigned long )(&array1[i_index_54]))));
      array1[i_index_54] = 1U;
    }
    else if (index == 1) {
      int i_index_55;
      (i_index_55 = index , v_Ret_array_bound_check_using_lookup_UL_Arg_Ul_Arg(((unsigned long long )(&array1)),((unsigned long )(&array1[i_index_55]))));
      int i_index_56;
      (i_index_56 = 0 , v_Ret_array_bound_check_using_lookup_UL_Arg_Ul_Arg(((unsigned long long )(&array1)),((unsigned long )(&array1[i_index_56]))));
      array1[i_index_55] = array1[i_index_56];
    }
    else {
      int i_index_57;
      (i_index_57 = index , v_Ret_array_bound_check_using_lookup_UL_Arg_Ul_Arg(((unsigned long long )(&array1)),((unsigned long )(&array1[i_index_57]))));
      int i_index_58;
      (i_index_58 = index - 1 , v_Ret_array_bound_check_using_lookup_UL_Arg_Ul_Arg(((unsigned long long )(&array1)),((unsigned long )(&array1[i_index_58]))));
      int i_index_59;
      (i_index_59 = index - 2 , v_Ret_array_bound_check_using_lookup_UL_Arg_Ul_Arg(((unsigned long long )(&array1)),((unsigned long )(&array1[i_index_59]))));
      array1[i_index_57] = (array1[i_index_58] + array1[i_index_59]);
    }
    int i_index_62;
    (i_index_62 = index , v_Ret_array_bound_check_using_lookup_UL_Arg_Ul_Arg(((unsigned long long )(&array1)),((unsigned long )(&array1[i_index_62]))));
    printf(__Pb__Cc__Pe___Ret_return_pointer___Pb__Cc__Pe___Type_Arg(__Pb__Cc__Pe___Type_Ret_Cast___Pb__Cc__Pe___Arg_UL_Arg_Ul_Arg_UL_Arg(((const char *)global_var60_183_10),((unsigned long long )(&global_var60_183_10)),((unsigned long )(sizeof(global_var60_183_10))),((unsigned long long )50))),index,array1[i_index_62]);
  }
  printf(__Pb__Cc__Pe___Ret_return_pointer___Pb__Cc__Pe___Type_Arg(__Pb__Cc__Pe___Type_Ret_Cast___Pb__Cc__Pe___Arg_UL_Arg_Ul_Arg_UL_Arg(((const char *)global_var63_186_9),((unsigned long long )(&global_var63_186_9)),((unsigned long )(sizeof(global_var63_186_9))),((unsigned long long )50))));
  v_Ret_free_overload___Pb__v__Pe___Type_Arg(__Pb__v__Pe___Type_Ret_Cast___Pb__Ui__Pe___Arg_UL_Arg(array1,((unsigned long long )(&array1))));
  return __Pb__v__Pe___Type_Ret_Cast___Pb__Ui__Pe___Arg_UL_Arg(array1,((unsigned long long )(&array1)));
}
static char* global_var65_195_9 = "%s\n";
static char* global_var72_204_10 = "\t\t\t\tarray2[%d] = %d\n";
static char* global_var75_207_9 = "\t\t\t\tfreeing array2\n";

struct __Pb__v__Pe___Type LinearScan(struct __Pb__v__Pe___Type threadid_str)
{
  (v_Ret_create_entry_UL_Arg_UL_Arg(((unsigned long long )(&threadid_str.ptr)),threadid_str.addr) , threadid_str.addr = ((unsigned long long )(&threadid_str.ptr)));
  printf(__Pb__Cc__Pe___Ret_return_pointer___Pb__Cc__Pe___Type_Arg(__Pb__Cc__Pe___Type_Ret_Cast___Pb__Cc__Pe___Arg_UL_Arg_Ul_Arg_UL_Arg(((const char *)global_var65_195_9),((unsigned long long )(&global_var65_195_9)),((unsigned long )(sizeof(global_var65_195_9))),((unsigned long long )50))),__Pb__c__Pe___Ret_return_pointer___Pb__c__Pe___Type_Arg(__Pb__c__Pe___Type_Ret_Cast___Pb__v__Pe___Type_Arg(threadid_str)));
  for (int index = 0; index < 100; index++) {
    if (index == 0) {
      int i_index_67;
      (i_index_67 = index , v_Ret_array_bound_check_using_lookup_UL_Arg_Ul_Arg(((unsigned long long )(&array2)),((unsigned long )(&array2[i_index_67]))));
      int i_index_68;
      (i_index_68 = index , v_Ret_array_bound_check_using_lookup_UL_Arg_Ul_Arg(((unsigned long long )(&array1)),((unsigned long )(&array1[i_index_68]))));
      array2[i_index_67] = array1[i_index_68];
    }
    else {
      int i_index_69;
      (i_index_69 = index , v_Ret_array_bound_check_using_lookup_UL_Arg_Ul_Arg(((unsigned long long )(&array2)),((unsigned long )(&array2[i_index_69]))));
      int i_index_70;
      (i_index_70 = index , v_Ret_array_bound_check_using_lookup_UL_Arg_Ul_Arg(((unsigned long long )(&array1)),((unsigned long )(&array1[i_index_70]))));
      int i_index_71;
      (i_index_71 = index - 1 , v_Ret_array_bound_check_using_lookup_UL_Arg_Ul_Arg(((unsigned long long )(&array1)),((unsigned long )(&array1[i_index_71]))));
      array2[i_index_69] = (array1[i_index_70] + array1[i_index_71]);
    }
    int i_index_74;
    (i_index_74 = index , v_Ret_array_bound_check_using_lookup_UL_Arg_Ul_Arg(((unsigned long long )(&array2)),((unsigned long )(&array2[i_index_74]))));
    printf(__Pb__Cc__Pe___Ret_return_pointer___Pb__Cc__Pe___Type_Arg(__Pb__Cc__Pe___Type_Ret_Cast___Pb__Cc__Pe___Arg_UL_Arg_Ul_Arg_UL_Arg(((const char *)global_var72_204_10),((unsigned long long )(&global_var72_204_10)),((unsigned long )(sizeof(global_var72_204_10))),((unsigned long long )50))),index,array2[i_index_74]);
  }
  printf(__Pb__Cc__Pe___Ret_return_pointer___Pb__Cc__Pe___Type_Arg(__Pb__Cc__Pe___Type_Ret_Cast___Pb__Cc__Pe___Arg_UL_Arg_Ul_Arg_UL_Arg(((const char *)global_var75_207_9),((unsigned long long )(&global_var75_207_9)),((unsigned long )(sizeof(global_var75_207_9))),((unsigned long long )50))));
  v_Ret_free_overload___Pb__v__Pe___Type_Arg(__Pb__v__Pe___Type_Ret_Cast___Pb__Ui__Pe___Arg_UL_Arg(array2,((unsigned long long )(&array2))));
  return __Pb__v__Pe___Type_Ret_Cast___Pb__Ui__Pe___Arg_UL_Arg(array2,((unsigned long long )(&array2)));
}

struct __Pb__Ui__Pe___Type 
{
  unsigned int *ptr;
  unsigned long long addr;
}
;
static struct __Pb__Ui__Pe___Type __Pb__Ui__Pe___Type_Ret_Cast___Pb__v__Pe___Type_Arg(struct __Pb__v__Pe___Type );

static struct __Pb__Ui__Pe___Type __Pb__Ui__Pe___Type_Ret_Cast___Pb__v__Pe___Type_Arg(struct __Pb__v__Pe___Type input1)
{
  struct __Pb__Ui__Pe___Type output;
  output.ptr = ((unsigned int *)input1.ptr);
  output.addr = input1.addr;
  return output;
}
static struct __Pb__Ui__Pe___Type __Pb__Ui__Pe___Type_Ret_assign_and_copy___Pb____Pb__Ui__Pe____Pe___Arg___Pb__Ui__Pe___Type_Arg(unsigned int **,struct __Pb__Ui__Pe___Type );
static struct __Pb__Ui__Pe___Type __Pb__Ui__Pe___Type_Ret_create_struct___Pb__Ui__Pe___Arg_UL_Arg(unsigned int *,unsigned long long );

static struct __Pb__Ui__Pe___Type __Pb__Ui__Pe___Type_Ret_create_struct___Pb__Ui__Pe___Arg_UL_Arg(unsigned int *input1,unsigned long long input2)
{
  struct __Pb__Ui__Pe___Type output;
  output.ptr = input1;
  output.addr = input2;
  return output;
}

static struct __Pb__Ui__Pe___Type __Pb__Ui__Pe___Type_Ret_assign_and_copy___Pb____Pb__Ui__Pe____Pe___Arg___Pb__Ui__Pe___Type_Arg(unsigned int **input1,struct __Pb__Ui__Pe___Type input2)
{
   *input1 = input2.ptr;
  v_Ret_create_entry_UL_Arg_UL_Arg(((unsigned long long )input1),((unsigned long long )input2.addr));
  return __Pb__Ui__Pe___Type_Ret_create_struct___Pb__Ui__Pe___Arg_UL_Arg(input2.ptr,((unsigned long long )input1));
}

struct __Pb__pthread_tL27R__typedef_declaration__Pe___Type 
{
  pthread_t *ptr;
  unsigned long long addr;
}
;
static struct __Pb__pthread_tL27R__typedef_declaration__Pe___Type L65R_Ret_AddressOf_UL_Arg___Pb____Pb___opaque_pthread_t__Pe____Pe___Arg_Ul_Arg_UL_Arg(unsigned long long ,struct _opaque_pthread_t **,unsigned long ,unsigned long long );
static struct __Pb__pthread_tL27R__typedef_declaration__Pe___Type L65R_Ret_create_struct___Pb____Pb___opaque_pthread_t__Pe____Pe___Arg_UL_Arg(struct _opaque_pthread_t **,unsigned long long );

static struct __Pb__pthread_tL27R__typedef_declaration__Pe___Type L65R_Ret_create_struct___Pb____Pb___opaque_pthread_t__Pe____Pe___Arg_UL_Arg(struct _opaque_pthread_t **input1,unsigned long long input2)
{
  struct __Pb__pthread_tL27R__typedef_declaration__Pe___Type output;
  output.ptr = input1;
  output.addr = input2;
  return output;
}

static struct __Pb__pthread_tL27R__typedef_declaration__Pe___Type L65R_Ret_AddressOf_UL_Arg___Pb____Pb___opaque_pthread_t__Pe____Pe___Arg_Ul_Arg_UL_Arg(unsigned long long input1,struct _opaque_pthread_t **input2,unsigned long input3,unsigned long long input4)
{
  v_Ret_create_entry_UL_Arg_UL_Arg_Ul_Arg_UL_Arg(input1,((unsigned long long )input2),input3,input4);
  return L65R_Ret_create_struct___Pb____Pb___opaque_pthread_t__Pe____Pe___Arg_UL_Arg(input2,input1);
}
static char* global_var77_226_44 = "fibonacci thread";
static struct __Pb__v__Pe___Type __Pb__v__Pe___Type_Ret_Cast___Pb__v__Pe___Arg_UL_Arg_Ul_Arg_UL_Arg(void *,unsigned long long ,unsigned long ,unsigned long long );
static struct __Pb__v__Pe___Type __Pb__v__Pe___Type_Ret_create_struct___Pb__v__Pe___Arg_UL_Arg(void *,unsigned long long );

static struct __Pb__v__Pe___Type __Pb__v__Pe___Type_Ret_create_struct___Pb__v__Pe___Arg_UL_Arg(void *input1,unsigned long long input2)
{
  struct __Pb__v__Pe___Type output;
  output.ptr = input1;
  output.addr = input2;
  return output;
}

static struct __Pb__v__Pe___Type __Pb__v__Pe___Type_Ret_Cast___Pb__v__Pe___Arg_UL_Arg_Ul_Arg_UL_Arg(void *input1,unsigned long long input2,unsigned long input3,unsigned long long input4)
{
  v_Ret_create_entry_UL_Arg_UL_Arg_Ul_Arg_UL_Arg(input2,((unsigned long long )input1),input3,input4);
  return __Pb__v__Pe___Type_Ret_create_struct___Pb__v__Pe___Arg_UL_Arg(input1,input2);
}

static void *Fibonacci_wrap(void *input1)
{
  struct __Pb__v__Pe___Type output = Fibonacci( *((struct __Pb__v__Pe___Type *)input1));
  return output.ptr;
}
static pthread_t *__Pb__pthread_tL27R__typedef_declaration__Pe___Ret_return_pointer_L65R_Arg(struct __Pb__pthread_tL27R__typedef_declaration__Pe___Type );

static pthread_t *__Pb__pthread_tL27R__typedef_declaration__Pe___Ret_return_pointer_L65R_Arg(struct __Pb__pthread_tL27R__typedef_declaration__Pe___Type input1)
{
  return input1.ptr;
}
static char* global_var80_227_45 = "linear scan thread";

static void *LinearScan_wrap(void *input1)
{
  struct __Pb__v__Pe___Type output = LinearScan( *((struct __Pb__v__Pe___Type *)input1));
  return output.ptr;
}

struct __Pb____Pb__v__Pe____Pe___Type 
{
  void **ptr;
  unsigned long long addr;
}
;
static struct __Pb____Pb__v__Pe____Pe___Type __Pb____Pb__v__Pe____Pe___Type_Ret_AddressOf_UL_Arg___Pb____Pb__v__Pe____Pe___Arg_Ul_Arg_UL_Arg(unsigned long long ,void **,unsigned long ,unsigned long long );
static struct __Pb____Pb__v__Pe____Pe___Type __Pb____Pb__v__Pe____Pe___Type_Ret_create_struct___Pb____Pb__v__Pe____Pe___Arg_UL_Arg(void **,unsigned long long );

static struct __Pb____Pb__v__Pe____Pe___Type __Pb____Pb__v__Pe____Pe___Type_Ret_create_struct___Pb____Pb__v__Pe____Pe___Arg_UL_Arg(void **input1,unsigned long long input2)
{
  struct __Pb____Pb__v__Pe____Pe___Type output;
  output.ptr = input1;
  output.addr = input2;
  return output;
}

static struct __Pb____Pb__v__Pe____Pe___Type __Pb____Pb__v__Pe____Pe___Type_Ret_AddressOf_UL_Arg___Pb____Pb__v__Pe____Pe___Arg_Ul_Arg_UL_Arg(unsigned long long input1,void **input2,unsigned long input3,unsigned long long input4)
{
  v_Ret_create_entry_UL_Arg_UL_Arg_Ul_Arg_UL_Arg(input1,((unsigned long long )input2),input3,input4);
  return __Pb____Pb__v__Pe____Pe___Type_Ret_create_struct___Pb____Pb__v__Pe____Pe___Arg_UL_Arg(input2,input1);
}
static void **__Pb____Pb__v__Pe____Pe___Ret_return_pointer___Pb____Pb__v__Pe____Pe___Type_Arg(struct __Pb____Pb__v__Pe____Pe___Type );

static void **__Pb____Pb__v__Pe____Pe___Ret_return_pointer___Pb____Pb__v__Pe____Pe___Type_Arg(struct __Pb____Pb__v__Pe____Pe___Type input1)
{
  return input1.ptr;
}
void v_Ret_create_dummy_entry_UL_Arg(unsigned long long );
void v_Ret_execAtFirst();
void v_Ret_execAtLast();

int main(int argc,char *argv[])
{
//SendQ.init();
  v_Ret_execAtFirst();
  v_Ret_create_dummy_entry_UL_Arg(((unsigned long long )(&argv)));
  int index_argc;
  for (index_argc = 0; index_argc < argc; ++index_argc) 
    v_Ret_create_dummy_entry_UL_Arg(((unsigned long long )(&argv[index_argc])));
  finish = ((BOOLEAN )0U);
  init();
  __Pb__Ui__Pe___Type_Ret_assign_and_copy___Pb____Pb__Ui__Pe____Pe___Arg___Pb__Ui__Pe___Type_Arg(&array1,__Pb__Ui__Pe___Type_Ret_Cast___Pb__v__Pe___Type_Arg(__Pb__v__Pe___Type_Ret_malloc_overload_Ul_Arg(((unsigned long )400UL))));
  __Pb__Ui__Pe___Type_Ret_assign_and_copy___Pb____Pb__Ui__Pe____Pe___Arg___Pb__Ui__Pe___Type_Arg(&array2,__Pb__Ui__Pe___Type_Ret_Cast___Pb__v__Pe___Type_Arg(__Pb__v__Pe___Type_Ret_malloc_overload_Ul_Arg(((unsigned long )400UL))));
  pthread_t fibo_th;
  pthread_t lins_th;
  struct __Pb__v__Pe___Type __Pb__v__Pe___Type_pthread_arg_79 = __Pb__v__Pe___Type_Ret_Cast___Pb__v__Pe___Arg_UL_Arg_Ul_Arg_UL_Arg(((void *)global_var77_226_44),((unsigned long long )(&global_var77_226_44)),((unsigned long )(sizeof(global_var77_226_44))),((unsigned long long )50));
  pthread_create(__Pb__pthread_tL27R__typedef_declaration__Pe___Ret_return_pointer_L65R_Arg(L65R_Ret_AddressOf_UL_Arg___Pb____Pb___opaque_pthread_t__Pe____Pe___Arg_Ul_Arg_UL_Arg(((unsigned long long )1),&((struct _opaque_pthread_t *)fibo_th),((unsigned long )(sizeof(fibo_th))),((unsigned long long )50))),0,Fibonacci_wrap,((void *)(&__Pb__v__Pe___Type_pthread_arg_79)));
  struct __Pb__v__Pe___Type __Pb__v__Pe___Type_pthread_arg_82 = __Pb__v__Pe___Type_Ret_Cast___Pb__v__Pe___Arg_UL_Arg_Ul_Arg_UL_Arg(((void *)global_var80_227_45),((unsigned long long )(&global_var80_227_45)),((unsigned long )(sizeof(global_var80_227_45))),((unsigned long long )50));
  pthread_create(__Pb__pthread_tL27R__typedef_declaration__Pe___Ret_return_pointer_L65R_Arg(L65R_Ret_AddressOf_UL_Arg___Pb____Pb___opaque_pthread_t__Pe____Pe___Arg_Ul_Arg_UL_Arg(((unsigned long long )2),&((struct _opaque_pthread_t *)lins_th),((unsigned long )(sizeof(lins_th))),((unsigned long long )50))),0,LinearScan_wrap,((void *)(&__Pb__v__Pe___Type_pthread_arg_82)));
  void *status;
  pthread_join(fibo_th,__Pb____Pb__v__Pe____Pe___Ret_return_pointer___Pb____Pb__v__Pe____Pe___Type_Arg(__Pb____Pb__v__Pe____Pe___Type_Ret_AddressOf_UL_Arg___Pb____Pb__v__Pe____Pe___Arg_Ul_Arg_UL_Arg(((unsigned long long )3),&status,((unsigned long )(sizeof(status))),((unsigned long long )50))));
  pthread_join(lins_th,__Pb____Pb__v__Pe____Pe___Ret_return_pointer___Pb____Pb__v__Pe____Pe___Type_Arg(__Pb____Pb__v__Pe____Pe___Type_Ret_AddressOf_UL_Arg___Pb____Pb__v__Pe____Pe___Arg_Ul_Arg_UL_Arg(((unsigned long long )4),&status,((unsigned long )(sizeof(status))),((unsigned long long )50))));
  finish = ((BOOLEAN )1U);
    #if 0
    #if 0    
    #endif
    #endif
// Last thing for main to do - wait for other threads to finish
  pthread_exit(0);
  v_Ret_execAtLast();
  return 1;
}
