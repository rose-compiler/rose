#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <stdint.h>
#include <map>
#include <stack>
#include <list>
#if 1
#ifdef __cplusplus
extern "C" {
#endif
#endif
typedef struct std::pair< uint64_t  , uint64_t  > IntPair;
extern "C" { IntPair insertIntoSLK(); }
extern "C" { void removeFromSLK(); }
extern "C" { IntPair insert_lock(); }
extern "C" { void remove_lock(uint64_t lock_index); }
extern "C" { void execAtFirst(); }
extern "C" { void execAtLast(); }

struct node 
{
  struct node *next;
  struct node *prev;
  int val;
}
;
void fn3(struct node *copy_this_ref,struct node *);
void fn2(struct node *copy_this_ref,struct node *);

struct __Pb__i__Pe___Type 
{
  int *ptr;
  unsigned long long addr;
}
;
struct __Pb__i__Pe___Type fn1(struct __Pb__i__Pe___Type temp_str,struct node *copy_this_ref);
void v_Ret_create_entry_UL_Arg_UL_Arg(unsigned long long ,unsigned long long );

static void rtc_copy(struct node *input1,struct node *input2)
{
  v_Ret_create_entry_UL_Arg_UL_Arg(((unsigned long long )(&input1 -> node::next)),((unsigned long long )(&input2 -> node::next))),v_Ret_create_entry_UL_Arg_UL_Arg(((unsigned long long )(&input1 -> node::prev)),((unsigned long long )(&input2 -> node::prev)));
}

struct __Pb__node__Pe___Type 
{
  struct node *ptr;
  unsigned long long addr;
}
;
static struct __Pb__node__Pe___Type __Pb__node__Pe___Type_Ret_create_struct___Pb__node__Pe___Arg_UL_Arg(struct node *,unsigned long long );

static struct __Pb__node__Pe___Type __Pb__node__Pe___Type_Ret_create_struct___Pb__node__Pe___Arg_UL_Arg(struct node *input1,unsigned long long input2)
{
  struct __Pb__node__Pe___Type output;
  output.__Pb__node__Pe___Type::ptr = input1;
  output.__Pb__node__Pe___Type::addr = input2;
  return output;
}
static struct node *__Pb__node__Pe___Ret_Deref___Pb__node__Pe___Type_Arg(struct __Pb__node__Pe___Type );
void v_Ret_check_entry_UL_Arg_UL_Arg(unsigned long long ,unsigned long long );

static struct node *__Pb__node__Pe___Ret_Deref___Pb__node__Pe___Type_Arg(struct __Pb__node__Pe___Type input1)
{
  v_Ret_check_entry_UL_Arg_UL_Arg(((unsigned long long )input1.__Pb__node__Pe___Type::ptr),input1.__Pb__node__Pe___Type::addr);
  return input1.__Pb__node__Pe___Type::ptr;
}

void fn3(struct node *copy_this_ref,struct node *retvar0)
{
  struct node copy_this_ref_local =  *copy_this_ref;
  rtc_copy(&copy_this_ref_local,copy_this_ref);
  printf("fn3\n");
  printf("copy_this: next_val: %u, prev_val: %u, val: %u\n",( *__Pb__node__Pe___Ret_Deref___Pb__node__Pe___Type_Arg(__Pb__node__Pe___Type_Ret_create_struct___Pb__node__Pe___Arg_UL_Arg(copy_this_ref_local.node::next,((unsigned long long )(&copy_this_ref_local.node::next))))).node::val,( *__Pb__node__Pe___Ret_Deref___Pb__node__Pe___Type_Arg(__Pb__node__Pe___Type_Ret_create_struct___Pb__node__Pe___Arg_UL_Arg(copy_this_ref_local.node::prev,((unsigned long long )(&copy_this_ref_local.node::prev))))).node::val,copy_this_ref_local.node::val);
   *retvar0 = copy_this_ref_local;
  return ;
}

void fn2(struct node *copy_this_ref,struct node *retvar1)
{
  struct node copy_this_ref_local =  *copy_this_ref;
  rtc_copy(&copy_this_ref_local,copy_this_ref);
  printf("fn2\n");
  printf("copy_this: next_val: %u, prev_val: %u, val: %u\n",( *__Pb__node__Pe___Ret_Deref___Pb__node__Pe___Type_Arg(__Pb__node__Pe___Type_Ret_create_struct___Pb__node__Pe___Arg_UL_Arg(copy_this_ref_local.node::next,((unsigned long long )(&copy_this_ref_local.node::next))))).node::val,( *__Pb__node__Pe___Ret_Deref___Pb__node__Pe___Type_Arg(__Pb__node__Pe___Type_Ret_create_struct___Pb__node__Pe___Arg_UL_Arg(copy_this_ref_local.node::prev,((unsigned long long )(&copy_this_ref_local.node::prev))))).node::val,copy_this_ref_local.node::val);
  struct node retvar_2;
  (::fn3(&copy_this_ref_local,&retvar_2) , retvar_2);
   *retvar1 = copy_this_ref_local;
  return ;
}
//int* fn1(int *temp, int lock, struct node copy_this) {
//    insertIntoSLK();

struct __Pb__v__Pe___Type 
{
  void *ptr;
  unsigned long long addr;
}
;
static struct __Pb__v__Pe___Type __Pb__v__Pe___Type_Ret_Cast___Pb__i__Pe___Type_Arg(struct __Pb__i__Pe___Type );

static struct __Pb__v__Pe___Type __Pb__v__Pe___Type_Ret_Cast___Pb__i__Pe___Type_Arg(struct __Pb__i__Pe___Type input1)
{
  struct __Pb__v__Pe___Type output;
  output.__Pb__v__Pe___Type::ptr = ((void *)input1.__Pb__i__Pe___Type::ptr);
  output.__Pb__v__Pe___Type::addr = input1.__Pb__i__Pe___Type::addr;
  return output;
}
void v_Ret_free_overload___Pb__v__Pe___Type_Arg(struct __Pb__v__Pe___Type );

struct __Pb__i__Pe___Type fn1(struct __Pb__i__Pe___Type temp_str,struct node *copy_this_ref)
{
  struct node copy_this_ref_local =  *copy_this_ref;
  rtc_copy(&copy_this_ref_local,copy_this_ref);
  (v_Ret_create_entry_UL_Arg_UL_Arg(((unsigned long long )(&temp_str.__Pb__i__Pe___Type::ptr)),temp_str.__Pb__i__Pe___Type::addr) , temp_str.__Pb__i__Pe___Type::addr = ((unsigned long long )(&temp_str.__Pb__i__Pe___Type::ptr)));
  insertIntoSLK();
  printf("copy_this: next_val: %u, prev_val: %u, val: %u\n",( *__Pb__node__Pe___Ret_Deref___Pb__node__Pe___Type_Arg(__Pb__node__Pe___Type_Ret_create_struct___Pb__node__Pe___Arg_UL_Arg(copy_this_ref_local.node::next,((unsigned long long )(&copy_this_ref_local.node::next))))).node::val,( *__Pb__node__Pe___Ret_Deref___Pb__node__Pe___Type_Arg(__Pb__node__Pe___Type_Ret_create_struct___Pb__node__Pe___Arg_UL_Arg(copy_this_ref_local.node::prev,((unsigned long long )(&copy_this_ref_local.node::prev))))).node::val,copy_this_ref_local.node::val);
  v_Ret_free_overload___Pb__v__Pe___Type_Arg(__Pb__v__Pe___Type_Ret_Cast___Pb__i__Pe___Type_Arg(temp_str));
//    remove_lock(lock);
  struct node retvar_3;
  (::fn2(&copy_this_ref_local,&retvar_3) , retvar_3);
  removeFromSLK();
  return temp_str;
}
struct __Pb__v__Pe___Type __Pb__v__Pe___Type_Ret_malloc_overload_Ul_Arg(unsigned long );
static struct __Pb__i__Pe___Type __Pb__i__Pe___Type_Ret_Cast___Pb__v__Pe___Type_Arg(struct __Pb__v__Pe___Type );

static struct __Pb__i__Pe___Type __Pb__i__Pe___Type_Ret_Cast___Pb__v__Pe___Type_Arg(struct __Pb__v__Pe___Type input1)
{
  struct __Pb__i__Pe___Type output;
  output.__Pb__i__Pe___Type::ptr = ((int *)input1.__Pb__v__Pe___Type::ptr);
  output.__Pb__i__Pe___Type::addr = input1.__Pb__v__Pe___Type::addr;
  return output;
}
static struct __Pb__i__Pe___Type __Pb__i__Pe___Type_Ret_Assign___Pb____Pb__i__Pe___Type__Pe___Arg___Pb__i__Pe___Type_Arg(struct __Pb__i__Pe___Type *,struct __Pb__i__Pe___Type );

static struct __Pb__i__Pe___Type __Pb__i__Pe___Type_Ret_Assign___Pb____Pb__i__Pe___Type__Pe___Arg___Pb__i__Pe___Type_Arg(struct __Pb__i__Pe___Type *input1,struct __Pb__i__Pe___Type input2)
{
  input1 -> __Pb__i__Pe___Type::ptr = input2.__Pb__i__Pe___Type::ptr;
  v_Ret_create_entry_UL_Arg_UL_Arg(input1 -> __Pb__i__Pe___Type::addr,input2.__Pb__i__Pe___Type::addr);
  return  *input1;
}
static struct __Pb__node__Pe___Type __Pb__node__Pe___Type_Ret_AddressOf_UL_Arg_Ui_Arg(unsigned long long ,unsigned int );
void v_Ret_create_entry_UL_Arg_UL_Arg_Ul_Arg(unsigned long long ,unsigned long long ,unsigned long );

static struct __Pb__node__Pe___Type __Pb__node__Pe___Type_Ret_AddressOf_UL_Arg_Ui_Arg(unsigned long long input1,unsigned int input2)
{
  struct __Pb__node__Pe___Type output;
  output.__Pb__node__Pe___Type::ptr = ((struct node *)input1);
  output.__Pb__node__Pe___Type::addr = ((unsigned long long )(&output.__Pb__node__Pe___Type::ptr));
  v_Ret_create_entry_UL_Arg_UL_Arg_Ul_Arg(output.__Pb__node__Pe___Type::addr,((unsigned long long )output.__Pb__node__Pe___Type::ptr),((unsigned long )input2));
  return output;
}
static struct __Pb__node__Pe___Type __Pb__node__Pe___Type_Ret_Assign___Pb____Pb__node__Pe___Type__Pe___Arg___Pb__node__Pe___Type_Arg(struct __Pb__node__Pe___Type *,struct __Pb__node__Pe___Type );

static struct __Pb__node__Pe___Type __Pb__node__Pe___Type_Ret_Assign___Pb____Pb__node__Pe___Type__Pe___Arg___Pb__node__Pe___Type_Arg(struct __Pb__node__Pe___Type *input1,struct __Pb__node__Pe___Type input2)
{
  input1 -> __Pb__node__Pe___Type::ptr = input2.__Pb__node__Pe___Type::ptr;
  v_Ret_create_entry_UL_Arg_UL_Arg(input1 -> __Pb__node__Pe___Type::addr,input2.__Pb__node__Pe___Type::addr);
  return  *input1;
}
static struct __Pb__i__Pe___Type __Pb__i__Pe___Type_Ret_create_struct___Pb__i__Pe___Arg_UL_Arg(int *,unsigned long long );

static struct __Pb__i__Pe___Type __Pb__i__Pe___Type_Ret_create_struct___Pb__i__Pe___Arg_UL_Arg(int *input1,unsigned long long input2)
{
  struct __Pb__i__Pe___Type output;
  output.__Pb__i__Pe___Type::ptr = input1;
  output.__Pb__i__Pe___Type::addr = input2;
  return output;
}
static int *__Pb__i__Pe___Ret_Deref___Pb__i__Pe___Type_Arg(struct __Pb__i__Pe___Type );

static int *__Pb__i__Pe___Ret_Deref___Pb__i__Pe___Type_Arg(struct __Pb__i__Pe___Type input1)
{
  v_Ret_check_entry_UL_Arg_UL_Arg(((unsigned long long )input1.__Pb__i__Pe___Type::ptr),input1.__Pb__i__Pe___Type::addr);
  return input1.__Pb__i__Pe___Type::ptr;
}

int main()
{
  execAtFirst();
  int *temp;
  struct __Pb__i__Pe___Type temp_str;
  ((temp_str.__Pb__i__Pe___Type::ptr = temp , temp_str.__Pb__i__Pe___Type::addr = ((unsigned long long )(&temp))) , ((__Pb__i__Pe___Type_Ret_Assign___Pb____Pb__i__Pe___Type__Pe___Arg___Pb__i__Pe___Type_Arg(&temp_str,__Pb__i__Pe___Type_Ret_Cast___Pb__v__Pe___Type_Arg(__Pb__v__Pe___Type_Ret_malloc_overload_Ul_Arg(((unsigned long )4UL)))) , temp = temp_str.__Pb__i__Pe___Type::ptr)));
//    IntPair lock_key = insert_lock();
  struct node head;
  head.node::val = 1;
  struct node next;
  next.node::val = 2;
  struct __Pb__node__Pe___Type __Pb__node__Pe___Type_Var_ovl_4;
  (((__Pb__node__Pe___Type_Var_ovl_4 = __Pb__node__Pe___Type_Ret_create_struct___Pb__node__Pe___Arg_UL_Arg(next.node::prev,((unsigned long long )(&next.node::prev))) , __Pb__node__Pe___Type_Ret_Assign___Pb____Pb__node__Pe___Type__Pe___Arg___Pb__node__Pe___Type_Arg(&__Pb__node__Pe___Type_Var_ovl_4,__Pb__node__Pe___Type_Ret_AddressOf_UL_Arg_Ui_Arg(((unsigned long long )(&head)),sizeof(head)))) ,  *((struct node **)__Pb__node__Pe___Type_Var_ovl_4.__Pb__node__Pe___Type::addr) = __Pb__node__Pe___Type_Var_ovl_4.__Pb__node__Pe___Type::ptr) , __Pb__node__Pe___Type_Var_ovl_4);
  struct node third;
  third.node::val = 3;
  struct __Pb__node__Pe___Type __Pb__node__Pe___Type_Var_ovl_5;
  (((__Pb__node__Pe___Type_Var_ovl_5 = __Pb__node__Pe___Type_Ret_create_struct___Pb__node__Pe___Arg_UL_Arg(third.node::prev,((unsigned long long )(&third.node::prev))) , __Pb__node__Pe___Type_Ret_Assign___Pb____Pb__node__Pe___Type__Pe___Arg___Pb__node__Pe___Type_Arg(&__Pb__node__Pe___Type_Var_ovl_5,__Pb__node__Pe___Type_Ret_AddressOf_UL_Arg_Ui_Arg(((unsigned long long )(&next)),sizeof(next)))) ,  *((struct node **)__Pb__node__Pe___Type_Var_ovl_5.__Pb__node__Pe___Type::addr) = __Pb__node__Pe___Type_Var_ovl_5.__Pb__node__Pe___Type::ptr) , __Pb__node__Pe___Type_Var_ovl_5);
  struct __Pb__node__Pe___Type __Pb__node__Pe___Type_Var_ovl_6;
  (((__Pb__node__Pe___Type_Var_ovl_6 = __Pb__node__Pe___Type_Ret_create_struct___Pb__node__Pe___Arg_UL_Arg(next.node::next,((unsigned long long )(&next.node::next))) , __Pb__node__Pe___Type_Ret_Assign___Pb____Pb__node__Pe___Type__Pe___Arg___Pb__node__Pe___Type_Arg(&__Pb__node__Pe___Type_Var_ovl_6,__Pb__node__Pe___Type_Ret_AddressOf_UL_Arg_Ui_Arg(((unsigned long long )(&third)),sizeof(third)))) ,  *((struct node **)__Pb__node__Pe___Type_Var_ovl_6.__Pb__node__Pe___Type::addr) = __Pb__node__Pe___Type_Var_ovl_6.__Pb__node__Pe___Type::ptr) , __Pb__node__Pe___Type_Var_ovl_6);
   *__Pb__i__Pe___Ret_Deref___Pb__i__Pe___Type_Arg(__Pb__i__Pe___Type_Ret_create_struct___Pb__i__Pe___Arg_UL_Arg(temp,((unsigned long long )(&temp)))) = 3;
//    temp = fn1(temp, lock_key.first, next);
  struct __Pb__i__Pe___Type __Pb__i__Pe___Type_Var_ovl_7;
  (((__Pb__i__Pe___Type_Var_ovl_7 = __Pb__i__Pe___Type_Ret_create_struct___Pb__i__Pe___Arg_UL_Arg(temp,((unsigned long long )(&temp))) , __Pb__i__Pe___Type_Ret_Assign___Pb____Pb__i__Pe___Type__Pe___Arg___Pb__i__Pe___Type_Arg(&__Pb__i__Pe___Type_Var_ovl_7,::fn1(__Pb__i__Pe___Type_Ret_create_struct___Pb__i__Pe___Arg_UL_Arg(temp,((unsigned long long )(&temp))),&next))) ,  *((int **)__Pb__i__Pe___Type_Var_ovl_7.__Pb__i__Pe___Type::addr) = __Pb__i__Pe___Type_Var_ovl_7.__Pb__i__Pe___Type::ptr) , __Pb__i__Pe___Type_Var_ovl_7);
    #if 0
    #endif
  execAtLast();
  return 1;
}
#if 1
#ifdef __cplusplus
}
#endif
#endif
