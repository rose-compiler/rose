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

struct __Pb__i__Pe___Type 
{
  int *ptr;
  unsigned long long addr;
}
;
void v_Ret_create_entry_UL_Arg_UL_Arg(unsigned long long ,unsigned long long );

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

struct __Pb__i__Pe___Type fn1(struct __Pb__i__Pe___Type temp_str,int lock)
{
  (v_Ret_create_entry_UL_Arg_UL_Arg(((unsigned long long )(&temp_str.__Pb__i__Pe___Type::ptr)),temp_str.__Pb__i__Pe___Type::addr) , temp_str.__Pb__i__Pe___Type::addr = ((unsigned long long )(&temp_str.__Pb__i__Pe___Type::ptr)));
  insertIntoSLK();
  v_Ret_free_overload___Pb__v__Pe___Type_Arg(__Pb__v__Pe___Type_Ret_Cast___Pb__i__Pe___Type_Arg(temp_str));
  remove_lock(lock);
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
void v_Ret_check_entry_UL_Arg_UL_Arg(unsigned long long ,unsigned long long );

int main()
{
  execAtFirst();
  int *temp;
  struct __Pb__i__Pe___Type temp_str;
  ((temp_str.__Pb__i__Pe___Type::ptr = temp , temp_str.__Pb__i__Pe___Type::addr = ((unsigned long long )(&temp))) , ((__Pb__i__Pe___Type_Ret_Assign___Pb____Pb__i__Pe___Type__Pe___Arg___Pb__i__Pe___Type_Arg(&temp_str,__Pb__i__Pe___Type_Ret_Cast___Pb__v__Pe___Type_Arg(__Pb__v__Pe___Type_Ret_malloc_overload_Ul_Arg(((unsigned long )4UL)))) , temp = temp_str.__Pb__i__Pe___Type::ptr)));
  IntPair lock_key = insert_lock();
   *__Pb__i__Pe___Ret_Deref___Pb__i__Pe___Type_Arg(__Pb__i__Pe___Type_Ret_create_struct___Pb__i__Pe___Arg_UL_Arg(temp,((unsigned long long )(&temp)))) = 3;
  struct __Pb__i__Pe___Type __Pb__i__Pe___Type_Var_ovl_0;
  (((__Pb__i__Pe___Type_Var_ovl_0 = __Pb__i__Pe___Type_Ret_create_struct___Pb__i__Pe___Arg_UL_Arg(temp,((unsigned long long )(&temp))) , __Pb__i__Pe___Type_Ret_Assign___Pb____Pb__i__Pe___Type__Pe___Arg___Pb__i__Pe___Type_Arg(&__Pb__i__Pe___Type_Var_ovl_0,::fn1(__Pb__i__Pe___Type_Ret_create_struct___Pb__i__Pe___Arg_UL_Arg(temp,((unsigned long long )(&temp))),lock_key.std::pair< uint64_t ,uint64_t > ::first))) ,  *((int **)__Pb__i__Pe___Type_Var_ovl_0.__Pb__i__Pe___Type::addr) = __Pb__i__Pe___Type_Var_ovl_0.__Pb__i__Pe___Type::ptr) , __Pb__i__Pe___Type_Var_ovl_0);
  v_Ret_free_overload___Pb__v__Pe___Type_Arg(__Pb__v__Pe___Type_Ret_Cast___Pb__i__Pe___Type_Arg(__Pb__i__Pe___Type_Ret_create_struct___Pb__i__Pe___Arg_UL_Arg(temp,((unsigned long long )(&temp)))));
   *__Pb__i__Pe___Ret_Deref___Pb__i__Pe___Type_Arg(__Pb__i__Pe___Type_Ret_create_struct___Pb__i__Pe___Arg_UL_Arg(temp,((unsigned long long )(&temp)))) = 2;
  execAtLast();
  return 1;
}
#if 1
#ifdef __cplusplus
}
#endif
#endif
