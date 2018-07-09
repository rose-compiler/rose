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
typedef enum BOOLEAN {FALSE,TRUE}BOOLEAN;
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
  output.__Pb__node__Pe___Type::ptr = input1;
  output.__Pb__node__Pe___Type::addr = input2;
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
  return input1 == input2.__Pb__node__Pe___Type::ptr;
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
static int global_var2_45_3 = 0;

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
  output.__Pb__v__Pe___Type::ptr = ((void *)input1);
  output.__Pb__v__Pe___Type::addr = input2;
  return output;
}
void v_Ret_free_overload___Pb__v__Pe___Type_Arg(struct __Pb__v__Pe___Type );
static int global_var4_47_16 = 0;
static struct __Pb__node__Pe___Type __Pb__node__Pe___Type_Ret_assign_and_copy___Pb____Pb__node__Pe____Pe___Arg___Pb__node__Pe___Type_Arg(struct node **,struct __Pb__node__Pe___Type );
void v_Ret_create_entry_UL_Arg_UL_Arg(unsigned long long ,unsigned long long );

static struct __Pb__node__Pe___Type __Pb__node__Pe___Type_Ret_assign_and_copy___Pb____Pb__node__Pe____Pe___Arg___Pb__node__Pe___Type_Arg(struct node **input1,struct __Pb__node__Pe___Type input2)
{
   *input1 = input2.__Pb__node__Pe___Type::ptr;
  v_Ret_create_entry_UL_Arg_UL_Arg(((unsigned long long )input1),((unsigned long long )input2.__Pb__node__Pe___Type::addr));
  return __Pb__node__Pe___Type_Ret_create_struct___Pb__node__Pe___Arg_UL_Arg(input2.__Pb__node__Pe___Type::ptr,((unsigned long long )input1));
}
static int global_var6_48_15 = 0;
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
    printf("empty list. Nothing to be popped. Quitting\n");
    exit(1);
  }
  else if (i_Ret_Equality___Pb__node__Pe___Arg___Pb__node__Pe___Arg(front_node,back_node)) {
// single node in the list
    (__builtin_expect((!i_Ret_Equality___Pb__node__Pe___Arg___Pb__node__Pe___Type_Arg(__Pb__node__Pe___Ret_deref_check___Pb__node__Pe___Arg_UL_Arg(front_node,((unsigned long long )(&front_node))) -> node::next,__Pb__node__Pe___Type_Ret_Cast___Pb__node__Pe___Arg_UL_Arg_Ul_Arg_UL_Arg(((struct node *)global_var2_45_3),((unsigned long long )(&global_var2_45_3)),((unsigned long )(sizeof(global_var2_45_3))),((unsigned long long )50)))),0L))?__assert_rtn(__func__,"/Users/vanka1/research/compilers/rose/rose_build/projects/RTC/pthread_pointer_example.cpp",45,"front_node->next == NULL") : ((void )0);
    v_Ret_free_overload___Pb__v__Pe___Type_Arg(__Pb__v__Pe___Type_Ret_Cast___Pb__node__Pe___Arg_UL_Arg(front_node,((unsigned long long )(&front_node))));
    __Pb__node__Pe___Type_Ret_assign_and_copy___Pb____Pb__node__Pe____Pe___Arg___Pb__node__Pe___Type_Arg(&front_node,__Pb__node__Pe___Type_Ret_Cast___Pb__node__Pe___Arg_UL_Arg_Ul_Arg_UL_Arg(((struct node *)global_var4_47_16),((unsigned long long )(&global_var4_47_16)),((unsigned long )(sizeof(global_var4_47_16))),((unsigned long long )50)));
    __Pb__node__Pe___Type_Ret_assign_and_copy___Pb____Pb__node__Pe____Pe___Arg___Pb__node__Pe___Type_Arg(&back_node,__Pb__node__Pe___Type_Ret_Cast___Pb__node__Pe___Arg_UL_Arg_Ul_Arg_UL_Arg(((struct node *)global_var6_48_15),((unsigned long long )(&global_var6_48_15)),((unsigned long )(sizeof(global_var6_48_15))),((unsigned long long )50)));
  }
  else {
    struct node *popped;
    __Pb__node__Pe___Type_Ret_assign_and_copy___Pb____Pb__node__Pe____Pe___Arg___Pb__node__Pe___Arg_UL_Arg(&popped,((struct node *)front_node),((unsigned long long )(&front_node)));
    __Pb__node__Pe___Type_Ret_assign_and_copy___Pb____Pb__node__Pe____Pe___Arg___Pb__node__Pe___Arg_UL_Arg(&front_node,((struct node *)(__Pb__node__Pe___Ret_deref_check___Pb__node__Pe___Arg_UL_Arg(front_node,((unsigned long long )(&front_node))) -> node::next)),((unsigned long long )(&__Pb__node__Pe___Ret_deref_check___Pb__node__Pe___Arg_UL_Arg(front_node,((unsigned long long )(&front_node))) -> node::next)));
    v_Ret_free_overload___Pb__v__Pe___Type_Arg(__Pb__v__Pe___Type_Ret_Cast___Pb__node__Pe___Arg_UL_Arg(popped,((unsigned long long )(&popped))));
  }
}
// push at the back
static int global_var8_59_18 = 0;
struct __Pb__v__Pe___Type __Pb__v__Pe___Type_Ret_malloc_overload_Ul_Arg(unsigned long );
static struct __Pb__node__Pe___Type __Pb__node__Pe___Type_Ret_Cast___Pb__v__Pe___Type_Arg(struct __Pb__v__Pe___Type );

static struct __Pb__node__Pe___Type __Pb__node__Pe___Type_Ret_Cast___Pb__v__Pe___Type_Arg(struct __Pb__v__Pe___Type input1)
{
  struct __Pb__node__Pe___Type output;
  output.__Pb__node__Pe___Type::ptr = ((struct node *)input1.__Pb__v__Pe___Type::ptr);
  output.__Pb__node__Pe___Type::addr = input1.__Pb__v__Pe___Type::addr;
  return output;
}
static int global_var10_62_21 = 0;
static int global_var12_65_3 = 0;
static int global_var14_70_20 = 0;
static int global_var16_73_3 = 0;

void push(int val)
{
  if (i_Ret_Equality___Pb__node__Pe___Arg___Pb__node__Pe___Type_Arg(back_node,__Pb__node__Pe___Type_Ret_Cast___Pb__node__Pe___Arg_UL_Arg_Ul_Arg_UL_Arg(((struct node *)global_var8_59_18),((unsigned long long )(&global_var8_59_18)),((unsigned long )(sizeof(global_var8_59_18))),((unsigned long long )50)))) {
// empty list
    __Pb__node__Pe___Type_Ret_assign_and_copy___Pb____Pb__node__Pe____Pe___Arg___Pb__node__Pe___Type_Arg(&back_node,__Pb__node__Pe___Type_Ret_Cast___Pb__v__Pe___Type_Arg(__Pb__v__Pe___Type_Ret_malloc_overload_Ul_Arg(((unsigned long )16UL))));
    __Pb__node__Pe___Type_Ret_assign_and_copy___Pb____Pb__node__Pe____Pe___Arg___Pb__node__Pe___Type_Arg(&__Pb__node__Pe___Ret_deref_check___Pb__node__Pe___Arg_UL_Arg(back_node,((unsigned long long )(&back_node))) -> node::next,__Pb__node__Pe___Type_Ret_Cast___Pb__node__Pe___Arg_UL_Arg_Ul_Arg_UL_Arg(((struct node *)global_var10_62_21),((unsigned long long )(&global_var10_62_21)),((unsigned long )(sizeof(global_var10_62_21))),((unsigned long long )50)));
    __Pb__node__Pe___Ret_deref_check___Pb__node__Pe___Arg_UL_Arg(back_node,((unsigned long long )(&back_node))) -> node::val = val;
// make the front node point to the same node
    (__builtin_expect((!i_Ret_Equality___Pb__node__Pe___Arg___Pb__node__Pe___Type_Arg(front_node,__Pb__node__Pe___Type_Ret_Cast___Pb__node__Pe___Arg_UL_Arg_Ul_Arg_UL_Arg(((struct node *)global_var12_65_3),((unsigned long long )(&global_var12_65_3)),((unsigned long )(sizeof(global_var12_65_3))),((unsigned long long )50)))),0L))?__assert_rtn(__func__,"/Users/vanka1/research/compilers/rose/rose_build/projects/RTC/pthread_pointer_example.cpp",65,"front_node == NULL") : ((void )0);
    __Pb__node__Pe___Type_Ret_assign_and_copy___Pb____Pb__node__Pe____Pe___Arg___Pb__node__Pe___Arg_UL_Arg(&front_node,((struct node *)back_node),((unsigned long long )(&back_node)));
  }
  else {
    struct node *new_node;
    __Pb__node__Pe___Type_Ret_assign_and_copy___Pb____Pb__node__Pe____Pe___Arg___Pb__node__Pe___Type_Arg(&new_node,__Pb__node__Pe___Type_Ret_Cast___Pb__v__Pe___Type_Arg(__Pb__v__Pe___Type_Ret_malloc_overload_Ul_Arg(((unsigned long )16UL))));
    __Pb__node__Pe___Type_Ret_assign_and_copy___Pb____Pb__node__Pe____Pe___Arg___Pb__node__Pe___Type_Arg(&__Pb__node__Pe___Ret_deref_check___Pb__node__Pe___Arg_UL_Arg(new_node,((unsigned long long )(&new_node))) -> node::next,__Pb__node__Pe___Type_Ret_Cast___Pb__node__Pe___Arg_UL_Arg_Ul_Arg_UL_Arg(((struct node *)global_var14_70_20),((unsigned long long )(&global_var14_70_20)),((unsigned long )(sizeof(global_var14_70_20))),((unsigned long long )50)));
    __Pb__node__Pe___Ret_deref_check___Pb__node__Pe___Arg_UL_Arg(new_node,((unsigned long long )(&new_node))) -> node::val = val;
// make back_node->next point to the new node
    (__builtin_expect((!i_Ret_Equality___Pb__node__Pe___Arg___Pb__node__Pe___Type_Arg(__Pb__node__Pe___Ret_deref_check___Pb__node__Pe___Arg_UL_Arg(back_node,((unsigned long long )(&back_node))) -> node::next,__Pb__node__Pe___Type_Ret_Cast___Pb__node__Pe___Arg_UL_Arg_Ul_Arg_UL_Arg(((struct node *)global_var16_73_3),((unsigned long long )(&global_var16_73_3)),((unsigned long )(sizeof(global_var16_73_3))),((unsigned long long )50)))),0L))?__assert_rtn(__func__,"/Users/vanka1/research/compilers/rose/rose_build/projects/RTC/pthread_pointer_example.cpp",73,"back_node->next == NULL") : ((void )0);
    __Pb__node__Pe___Type_Ret_assign_and_copy___Pb____Pb__node__Pe____Pe___Arg___Pb__node__Pe___Arg_UL_Arg(&__Pb__node__Pe___Ret_deref_check___Pb__node__Pe___Arg_UL_Arg(back_node,((unsigned long long )(&back_node))) -> node::next,((struct node *)new_node),((unsigned long long )(&new_node)));
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
static int global_var18_92_15 = 0;
static int global_var20_93_14 = 0;

void init()
{
  __Pb__node__Pe___Type_Ret_assign_and_copy___Pb____Pb__node__Pe____Pe___Arg___Pb__node__Pe___Type_Arg(&front_node,__Pb__node__Pe___Type_Ret_Cast___Pb__node__Pe___Arg_UL_Arg_Ul_Arg_UL_Arg(((struct node *)global_var18_92_15),((unsigned long long )(&global_var18_92_15)),((unsigned long )(sizeof(global_var18_92_15))),((unsigned long long )50)));
  __Pb__node__Pe___Type_Ret_assign_and_copy___Pb____Pb__node__Pe____Pe___Arg___Pb__node__Pe___Type_Arg(&back_node,__Pb__node__Pe___Type_Ret_Cast___Pb__node__Pe___Arg_UL_Arg_Ul_Arg_UL_Arg(((struct node *)global_var20_93_14),((unsigned long long )(&global_var20_93_14)),((unsigned long )(sizeof(global_var20_93_14))),((unsigned long long )50)));
}
// check if empty
static int global_var22_98_19 = 0;
static int global_var24_99_3 = 0;

int empty()
{
  if (i_Ret_Equality___Pb__node__Pe___Arg___Pb__node__Pe___Type_Arg(front_node,__Pb__node__Pe___Type_Ret_Cast___Pb__node__Pe___Arg_UL_Arg_Ul_Arg_UL_Arg(((struct node *)global_var22_98_19),((unsigned long long )(&global_var22_98_19)),((unsigned long )(sizeof(global_var22_98_19))),((unsigned long long )50)))) {
    (__builtin_expect((!i_Ret_Equality___Pb__node__Pe___Arg___Pb__node__Pe___Type_Arg(back_node,__Pb__node__Pe___Type_Ret_Cast___Pb__node__Pe___Arg_UL_Arg_Ul_Arg_UL_Arg(((struct node *)global_var24_99_3),((unsigned long long )(&global_var24_99_3)),((unsigned long )(sizeof(global_var24_99_3))),((unsigned long long )50)))),0L))?__assert_rtn(__func__,"/Users/vanka1/research/compilers/rose/rose_build/projects/RTC/pthread_pointer_example.cpp",99,"back_node == NULL") : ((void )0);
    return 1;
  }
  else {
    return 0;
  }
}
// print queue
static int global_var26_119_18 = 0;
static int i_Ret_NotEqual___Pb__node__Pe___Arg___Pb__node__Pe___Type_Arg(struct node *,struct __Pb__node__Pe___Type );

static int i_Ret_NotEqual___Pb__node__Pe___Arg___Pb__node__Pe___Type_Arg(struct node *input1,struct __Pb__node__Pe___Type input2)
{
  return input1 != input2.__Pb__node__Pe___Type::ptr;
}

void printQ()
{
  if ((empty())) {
    printf("Queue is empty, nothing to print\n");
  }
  else {
    printf("Printing Q\n");
// go from front to back
    struct node *temp;
    __Pb__node__Pe___Type_Ret_assign_and_copy___Pb____Pb__node__Pe____Pe___Arg___Pb__node__Pe___Arg_UL_Arg(&temp,((struct node *)front_node),((unsigned long long )(&front_node)));
    do {
      printf("%d,",__Pb__node__Pe___Ret_deref_check___Pb__node__Pe___Arg_UL_Arg(temp,((unsigned long long )(&temp))) -> node::val);
      __Pb__node__Pe___Type_Ret_assign_and_copy___Pb____Pb__node__Pe____Pe___Arg___Pb__node__Pe___Arg_UL_Arg(&temp,((struct node *)(__Pb__node__Pe___Ret_deref_check___Pb__node__Pe___Arg_UL_Arg(temp,((unsigned long long )(&temp))) -> node::next)),((unsigned long long )(&__Pb__node__Pe___Ret_deref_check___Pb__node__Pe___Arg_UL_Arg(temp,((unsigned long long )(&temp))) -> node::next)));
    }while (i_Ret_NotEqual___Pb__node__Pe___Arg___Pb__node__Pe___Type_Arg(temp,__Pb__node__Pe___Type_Ret_Cast___Pb__node__Pe___Arg_UL_Arg_Ul_Arg_UL_Arg(((struct node *)global_var26_119_18),((unsigned long long )(&global_var26_119_18)),((unsigned long )(sizeof(global_var26_119_18))),((unsigned long long )50))));
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
  output.__Pb__c__Pe___Type::ptr = ((char *)input1.__Pb__v__Pe___Type::ptr);
  output.__Pb__c__Pe___Type::addr = input1.__Pb__v__Pe___Type::addr;
  return output;
}
static char *__Pb__c__Pe___Ret_return_pointer___Pb__c__Pe___Type_Arg(struct __Pb__c__Pe___Type );

static char *__Pb__c__Pe___Ret_return_pointer___Pb__c__Pe___Type_Arg(struct __Pb__c__Pe___Type input1)
{
  return input1.__Pb__c__Pe___Type::ptr;
}
void v_Ret_array_bound_check_using_lookup_UL_Arg_Ul_Arg(unsigned long long ,unsigned long );
static struct __Pb__v__Pe___Type __Pb__v__Pe___Type_Ret_Cast___Pb__Ui__Pe___Arg_UL_Arg(unsigned int *,unsigned long long );

static struct __Pb__v__Pe___Type __Pb__v__Pe___Type_Ret_Cast___Pb__Ui__Pe___Arg_UL_Arg(unsigned int *input1,unsigned long long input2)
{
  struct __Pb__v__Pe___Type output;
  output.__Pb__v__Pe___Type::ptr = ((void *)input1);
  output.__Pb__v__Pe___Type::addr = input2;
  return output;
}
static int global_var35_190_9 = 0;
static struct __Pb__v__Pe___Type __Pb__v__Pe___Type_Ret_Cast___Pb__v__Pe___Arg_UL_Arg_Ul_Arg_UL_Arg(void *,unsigned long long ,unsigned long ,unsigned long long );
static struct __Pb__v__Pe___Type __Pb__v__Pe___Type_Ret_create_struct___Pb__v__Pe___Arg_UL_Arg(void *,unsigned long long );

static struct __Pb__v__Pe___Type __Pb__v__Pe___Type_Ret_create_struct___Pb__v__Pe___Arg_UL_Arg(void *input1,unsigned long long input2)
{
  struct __Pb__v__Pe___Type output;
  output.__Pb__v__Pe___Type::ptr = input1;
  output.__Pb__v__Pe___Type::addr = input2;
  return output;
}

static struct __Pb__v__Pe___Type __Pb__v__Pe___Type_Ret_Cast___Pb__v__Pe___Arg_UL_Arg_Ul_Arg_UL_Arg(void *input1,unsigned long long input2,unsigned long input3,unsigned long long input4)
{
  v_Ret_create_entry_UL_Arg_UL_Arg_Ul_Arg_UL_Arg(input2,((unsigned long long )input1),input3,input4);
  return __Pb__v__Pe___Type_Ret_create_struct___Pb__v__Pe___Arg_UL_Arg(input1,input2);
}

struct __Pb__v__Pe___Type Fibonacci(struct __Pb__v__Pe___Type threadid_str)
{
  (v_Ret_create_entry_UL_Arg_UL_Arg(((unsigned long long )(&threadid_str.__Pb__v__Pe___Type::ptr)),threadid_str.__Pb__v__Pe___Type::addr) , threadid_str.__Pb__v__Pe___Type::addr = ((unsigned long long )(&threadid_str.__Pb__v__Pe___Type::ptr)));
  printf("%s\n",__Pb__c__Pe___Ret_return_pointer___Pb__c__Pe___Type_Arg(__Pb__c__Pe___Type_Ret_Cast___Pb__v__Pe___Type_Arg(threadid_str)));
  for (int index = 0; index < 100; index++) {
    if (index == 0) {
      int i_index_28;
      (i_index_28 = index , v_Ret_array_bound_check_using_lookup_UL_Arg_Ul_Arg(((unsigned long long )(&array1)),((unsigned long )(&array1[i_index_28]))));
      array1[i_index_28] = 1U;
    }
    else if (index == 1) {
      int i_index_29;
      (i_index_29 = index , v_Ret_array_bound_check_using_lookup_UL_Arg_Ul_Arg(((unsigned long long )(&array1)),((unsigned long )(&array1[i_index_29]))));
      int i_index_30;
      (i_index_30 = 0 , v_Ret_array_bound_check_using_lookup_UL_Arg_Ul_Arg(((unsigned long long )(&array1)),((unsigned long )(&array1[i_index_30]))));
      array1[i_index_29] = array1[i_index_30];
    }
    else {
      int i_index_31;
      (i_index_31 = index , v_Ret_array_bound_check_using_lookup_UL_Arg_Ul_Arg(((unsigned long long )(&array1)),((unsigned long )(&array1[i_index_31]))));
      int i_index_32;
      (i_index_32 = index - 1 , v_Ret_array_bound_check_using_lookup_UL_Arg_Ul_Arg(((unsigned long long )(&array1)),((unsigned long )(&array1[i_index_32]))));
      int i_index_33;
      (i_index_33 = index - 2 , v_Ret_array_bound_check_using_lookup_UL_Arg_Ul_Arg(((unsigned long long )(&array1)),((unsigned long )(&array1[i_index_33]))));
      array1[i_index_31] = (array1[i_index_32] + array1[i_index_33]);
    }
    int i_index_34;
    (i_index_34 = index , v_Ret_array_bound_check_using_lookup_UL_Arg_Ul_Arg(((unsigned long long )(&array1)),((unsigned long )(&array1[i_index_34]))));
    printf("array1[%d] = %d\n",index,array1[i_index_34]);
  }
  printf("freeing array1\n");
  v_Ret_free_overload___Pb__v__Pe___Type_Arg(__Pb__v__Pe___Type_Ret_Cast___Pb__Ui__Pe___Arg_UL_Arg(array1,((unsigned long long )(&array1))));
  return __Pb__v__Pe___Type_Ret_Cast___Pb__v__Pe___Arg_UL_Arg_Ul_Arg_UL_Arg(((void *)global_var35_190_9),((unsigned long long )(&global_var35_190_9)),((unsigned long )(sizeof(global_var35_190_9))),((unsigned long long )50));
}
static int global_var43_211_9 = 0;

struct __Pb__v__Pe___Type LinearScan(struct __Pb__v__Pe___Type threadid_str)
{
  (v_Ret_create_entry_UL_Arg_UL_Arg(((unsigned long long )(&threadid_str.__Pb__v__Pe___Type::ptr)),threadid_str.__Pb__v__Pe___Type::addr) , threadid_str.__Pb__v__Pe___Type::addr = ((unsigned long long )(&threadid_str.__Pb__v__Pe___Type::ptr)));
  printf("%s\n",__Pb__c__Pe___Ret_return_pointer___Pb__c__Pe___Type_Arg(__Pb__c__Pe___Type_Ret_Cast___Pb__v__Pe___Type_Arg(threadid_str)));
  for (int index = 0; index < 100; index++) {
    if (index == 0) {
      int i_index_37;
      (i_index_37 = index , v_Ret_array_bound_check_using_lookup_UL_Arg_Ul_Arg(((unsigned long long )(&array2)),((unsigned long )(&array2[i_index_37]))));
      int i_index_38;
      (i_index_38 = index , v_Ret_array_bound_check_using_lookup_UL_Arg_Ul_Arg(((unsigned long long )(&array1)),((unsigned long )(&array1[i_index_38]))));
      array2[i_index_37] = array1[i_index_38];
    }
    else {
      int i_index_39;
      (i_index_39 = index , v_Ret_array_bound_check_using_lookup_UL_Arg_Ul_Arg(((unsigned long long )(&array2)),((unsigned long )(&array2[i_index_39]))));
      int i_index_40;
      (i_index_40 = index , v_Ret_array_bound_check_using_lookup_UL_Arg_Ul_Arg(((unsigned long long )(&array1)),((unsigned long )(&array1[i_index_40]))));
      int i_index_41;
      (i_index_41 = index - 1 , v_Ret_array_bound_check_using_lookup_UL_Arg_Ul_Arg(((unsigned long long )(&array1)),((unsigned long )(&array1[i_index_41]))));
      array2[i_index_39] = (array1[i_index_40] + array1[i_index_41]);
    }
    int i_index_42;
    (i_index_42 = index , v_Ret_array_bound_check_using_lookup_UL_Arg_Ul_Arg(((unsigned long long )(&array2)),((unsigned long )(&array2[i_index_42]))));
    printf("\t\t\t\tarray2[%d] = %d\n",index,array2[i_index_42]);
  }
  printf("\t\t\t\tfreeing array2\n");
  v_Ret_free_overload___Pb__v__Pe___Type_Arg(__Pb__v__Pe___Type_Ret_Cast___Pb__Ui__Pe___Arg_UL_Arg(array2,((unsigned long long )(&array2))));
  return __Pb__v__Pe___Type_Ret_Cast___Pb__v__Pe___Arg_UL_Arg_Ul_Arg_UL_Arg(((void *)global_var43_211_9),((unsigned long long )(&global_var43_211_9)),((unsigned long )(sizeof(global_var43_211_9))),((unsigned long long )50));
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
  output.__Pb__Ui__Pe___Type::ptr = ((unsigned int *)input1.__Pb__v__Pe___Type::ptr);
  output.__Pb__Ui__Pe___Type::addr = input1.__Pb__v__Pe___Type::addr;
  return output;
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

struct __Pb__pthread_tL27R__typedef_declaration__Pe___Type 
{
  pthread_t *ptr;
  unsigned long long addr;
}
;
static struct __Pb__pthread_tL27R__typedef_declaration__Pe___Type L1340R_Ret_AddressOf_UL_Arg___Pb____Pb___opaque_pthread_t__Pe____Pe___Arg_Ul_Arg_UL_Arg(unsigned long long ,struct _opaque_pthread_t **,unsigned long ,unsigned long long );
static struct __Pb__pthread_tL27R__typedef_declaration__Pe___Type L1340R_Ret_create_struct___Pb____Pb___opaque_pthread_t__Pe____Pe___Arg_UL_Arg(struct _opaque_pthread_t **,unsigned long long );

static struct __Pb__pthread_tL27R__typedef_declaration__Pe___Type L1340R_Ret_create_struct___Pb____Pb___opaque_pthread_t__Pe____Pe___Arg_UL_Arg(struct _opaque_pthread_t **input1,unsigned long long input2)
{
  struct __Pb__pthread_tL27R__typedef_declaration__Pe___Type output;
  output.__Pb__pthread_tL27R__typedef_declaration__Pe___Type::ptr = input1;
  output.__Pb__pthread_tL27R__typedef_declaration__Pe___Type::addr = input2;
  return output;
}

static struct __Pb__pthread_tL27R__typedef_declaration__Pe___Type L1340R_Ret_AddressOf_UL_Arg___Pb____Pb___opaque_pthread_t__Pe____Pe___Arg_Ul_Arg_UL_Arg(unsigned long long input1,struct _opaque_pthread_t **input2,unsigned long input3,unsigned long long input4)
{
  v_Ret_create_entry_UL_Arg_UL_Arg_Ul_Arg_UL_Arg(input1,((unsigned long long )input2),input3,input4);
  return L1340R_Ret_create_struct___Pb____Pb___opaque_pthread_t__Pe____Pe___Arg_UL_Arg(input2,input1);
}
static int global_var45_226_27 = 0;

struct __Pb__Cpthread_attr_tL18R__typedef_declaration__Pe___Type 
{
  const pthread_attr_t *ptr;
  unsigned long long addr;
}
;
static struct __Pb__Cpthread_attr_tL18R__typedef_declaration__Pe___Type L1345R_Ret_Cast___Pb__Cpthread_attr_tL18R__typedef_declaration__Pe___Arg_UL_Arg_Ul_Arg_UL_Arg(const pthread_attr_t *,unsigned long long ,unsigned long ,unsigned long long );
static struct __Pb__Cpthread_attr_tL18R__typedef_declaration__Pe___Type L1345R_Ret_create_struct___Pb__Cpthread_attr_tL18R__typedef_declaration__Pe___Arg_UL_Arg(const pthread_attr_t *,unsigned long long );

static struct __Pb__Cpthread_attr_tL18R__typedef_declaration__Pe___Type L1345R_Ret_create_struct___Pb__Cpthread_attr_tL18R__typedef_declaration__Pe___Arg_UL_Arg(const pthread_attr_t *input1,unsigned long long input2)
{
  struct __Pb__Cpthread_attr_tL18R__typedef_declaration__Pe___Type output;
  output.__Pb__Cpthread_attr_tL18R__typedef_declaration__Pe___Type::ptr = input1;
  output.__Pb__Cpthread_attr_tL18R__typedef_declaration__Pe___Type::addr = input2;
  return output;
}

static struct __Pb__Cpthread_attr_tL18R__typedef_declaration__Pe___Type L1345R_Ret_Cast___Pb__Cpthread_attr_tL18R__typedef_declaration__Pe___Arg_UL_Arg_Ul_Arg_UL_Arg(const pthread_attr_t *input1,unsigned long long input2,unsigned long input3,unsigned long long input4)
{
  v_Ret_create_entry_UL_Arg_UL_Arg_Ul_Arg_UL_Arg(input2,((unsigned long long )input1),input3,input4);
  return L1345R_Ret_create_struct___Pb__Cpthread_attr_tL18R__typedef_declaration__Pe___Arg_UL_Arg(input1,input2);
}
static char* global_var47_226_44 = "fibonacci thread";
static pthread_t *__Pb__pthread_tL27R__typedef_declaration__Pe___Ret_return_pointer_L1340R_Arg(struct __Pb__pthread_tL27R__typedef_declaration__Pe___Type );

static pthread_t *__Pb__pthread_tL27R__typedef_declaration__Pe___Ret_return_pointer_L1340R_Arg(struct __Pb__pthread_tL27R__typedef_declaration__Pe___Type input1)
{
  return input1.__Pb__pthread_tL27R__typedef_declaration__Pe___Type::ptr;
}
static const pthread_attr_t *__Pb__Cpthread_attr_tL18R__typedef_declaration__Pe___Ret_return_pointer_L1345R_Arg(struct __Pb__Cpthread_attr_tL18R__typedef_declaration__Pe___Type );

static const pthread_attr_t *__Pb__Cpthread_attr_tL18R__typedef_declaration__Pe___Ret_return_pointer_L1345R_Arg(struct __Pb__Cpthread_attr_tL18R__typedef_declaration__Pe___Type input1)
{
  return input1.__Pb__Cpthread_attr_tL18R__typedef_declaration__Pe___Type::ptr;
}
static void *__Pb__v__Pe___Ret_return_pointer___Pb__v__Pe___Type_Arg(struct __Pb__v__Pe___Type );

static void *__Pb__v__Pe___Ret_return_pointer___Pb__v__Pe___Type_Arg(struct __Pb__v__Pe___Type input1)
{
  return input1.__Pb__v__Pe___Type::ptr;
}
static int global_var49_227_27 = 0;
static char* global_var51_227_45 = "linear scan thread";

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
  output.__Pb____Pb__v__Pe____Pe___Type::ptr = input1;
  output.__Pb____Pb__v__Pe____Pe___Type::addr = input2;
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
  return input1.__Pb____Pb__v__Pe____Pe___Type::ptr;
}
static int global_var53_279_15 = 0;
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
  finish = ((BOOLEAN )0);
  init();
  __Pb__Ui__Pe___Type_Ret_assign_and_copy___Pb____Pb__Ui__Pe____Pe___Arg___Pb__Ui__Pe___Type_Arg(&array1,__Pb__Ui__Pe___Type_Ret_Cast___Pb__v__Pe___Type_Arg(__Pb__v__Pe___Type_Ret_malloc_overload_Ul_Arg(((unsigned long )400UL))));
  __Pb__Ui__Pe___Type_Ret_assign_and_copy___Pb____Pb__Ui__Pe____Pe___Arg___Pb__Ui__Pe___Type_Arg(&array2,__Pb__Ui__Pe___Type_Ret_Cast___Pb__v__Pe___Type_Arg(__Pb__v__Pe___Type_Ret_malloc_overload_Ul_Arg(((unsigned long )400UL))));
  pthread_t fibo_th;
  pthread_t lins_th;
  pthread_create(__Pb__pthread_tL27R__typedef_declaration__Pe___Ret_return_pointer_L1340R_Arg(L1340R_Ret_AddressOf_UL_Arg___Pb____Pb___opaque_pthread_t__Pe____Pe___Arg_Ul_Arg_UL_Arg(((unsigned long long )1),&((struct _opaque_pthread_t *)fibo_th),((unsigned long )(sizeof(fibo_th))),((unsigned long long )50))),__Pb__Cpthread_attr_tL18R__typedef_declaration__Pe___Ret_return_pointer_L1345R_Arg(L1345R_Ret_Cast___Pb__Cpthread_attr_tL18R__typedef_declaration__Pe___Arg_UL_Arg_Ul_Arg_UL_Arg(((const pthread_attr_t *)global_var45_226_27),((unsigned long long )(&global_var45_226_27)),((unsigned long )(sizeof(global_var45_226_27))),((unsigned long long )50))),Fibonacci,__Pb__v__Pe___Ret_return_pointer___Pb__v__Pe___Type_Arg(__Pb__v__Pe___Type_Ret_Cast___Pb__v__Pe___Arg_UL_Arg_Ul_Arg_UL_Arg(((void *)global_var47_226_44),((unsigned long long )(&global_var47_226_44)),((unsigned long )(sizeof(global_var47_226_44))),((unsigned long long )50))));
  pthread_create(__Pb__pthread_tL27R__typedef_declaration__Pe___Ret_return_pointer_L1340R_Arg(L1340R_Ret_AddressOf_UL_Arg___Pb____Pb___opaque_pthread_t__Pe____Pe___Arg_Ul_Arg_UL_Arg(((unsigned long long )2),&((struct _opaque_pthread_t *)lins_th),((unsigned long )(sizeof(lins_th))),((unsigned long long )50))),__Pb__Cpthread_attr_tL18R__typedef_declaration__Pe___Ret_return_pointer_L1345R_Arg(L1345R_Ret_Cast___Pb__Cpthread_attr_tL18R__typedef_declaration__Pe___Arg_UL_Arg_Ul_Arg_UL_Arg(((const pthread_attr_t *)global_var49_227_27),((unsigned long long )(&global_var49_227_27)),((unsigned long )(sizeof(global_var49_227_27))),((unsigned long long )50))),LinearScan,__Pb__v__Pe___Ret_return_pointer___Pb__v__Pe___Type_Arg(__Pb__v__Pe___Type_Ret_Cast___Pb__v__Pe___Arg_UL_Arg_Ul_Arg_UL_Arg(((void *)global_var51_227_45),((unsigned long long )(&global_var51_227_45)),((unsigned long )(sizeof(global_var51_227_45))),((unsigned long long )50))));
  void *status;
  pthread_join(fibo_th,__Pb____Pb__v__Pe____Pe___Ret_return_pointer___Pb____Pb__v__Pe____Pe___Type_Arg(__Pb____Pb__v__Pe____Pe___Type_Ret_AddressOf_UL_Arg___Pb____Pb__v__Pe____Pe___Arg_Ul_Arg_UL_Arg(((unsigned long long )3),&status,((unsigned long )(sizeof(status))),((unsigned long long )50))));
  pthread_join(lins_th,__Pb____Pb__v__Pe____Pe___Ret_return_pointer___Pb____Pb__v__Pe____Pe___Type_Arg(__Pb____Pb__v__Pe____Pe___Type_Ret_AddressOf_UL_Arg___Pb____Pb__v__Pe____Pe___Arg_Ul_Arg_UL_Arg(((unsigned long long )4),&status,((unsigned long )(sizeof(status))),((unsigned long long )50))));
  finish = TRUE;
    #if 0
    #if 0    
    #endif
    #endif
// Last thing for main to do - wait for other threads to finish
  pthread_exit(__Pb__v__Pe___Ret_return_pointer___Pb__v__Pe___Type_Arg(__Pb__v__Pe___Type_Ret_Cast___Pb__v__Pe___Arg_UL_Arg_Ul_Arg_UL_Arg(((void *)global_var53_279_15),((unsigned long long )(&global_var53_279_15)),((unsigned long )(sizeof(global_var53_279_15))),((unsigned long long )50))));
  v_Ret_execAtLast();
  return 1;
}
