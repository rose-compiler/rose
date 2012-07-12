struct __Pb__i__Pe___Type 
{
  int *ptr;
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
#define LINKED_LIST_SIZE 10
#define ARRAY_SIZE 10
#if 0
//class Base* temp = new class Base[size];
#endif

#include "decls.h"

struct __Pb__Cc__Pe___Type 
{
  const char *ptr;
  unsigned long long addr;
}
;

struct __Pb__v__Pe___Type 
{
  void *ptr;
  unsigned long long addr;
}
;

struct __Pb__node__Pe___Type 
{
  struct node *ptr;
  unsigned long long addr;
}
;

struct __Pb__Derived2__Pe___Type 
{
  class Derived2 *ptr;
  unsigned long long addr;
}
;

struct __Pb__Base__Pe___Type 
{
  class Base *ptr;
  unsigned long long addr;
}
;

struct __Pb__Derived1__Pe___Type 
{
  class Derived1 *ptr;
  unsigned long long addr;
}
;

struct __Pb__f__Pe___Type 
{
  float *ptr;
  unsigned long long addr;
}
;


struct __Pb__Base__Pe___Type __Pb__Base__Pe___Type_Ret_Assign__Pb__Base__Pe___Type_Arg___Pb__Base__Pe___Arg_(unsigned long long ,class Base *);
struct __Pb__node__Pe___Type __Pb__node__Pe___Type_Ret_Cast__Pb__v__Pe___Type_Arg__PntrArr(struct __Pb__v__Pe___Type );
bool b_Ret_Equality__Pb__node__Pe___Type_Arg___Pb__node__Pe___Type_Arg_(struct __Pb__node__Pe___Type ,struct __Pb__node__Pe___Type );
struct __Pb__node__Pe___Type __Pb__node__Pe___Type_Ret_DotExpnode_Arg___Pb__node__Pe___Arg_(struct node *,unsigned long long );
struct __Pb__Cc__Pe___Type create_struct___Pb__Cc__Pe___Arg__UL_Arg__Ul_Arg____Pb__Cc__Pe___Type_Ret_(const char *,unsigned long long ,unsigned long );

struct __Pb__Base__Pe___Type __Pb__Base__Pe___Type_Ret_Cast__Pb__v__Pe___Type_Arg__PntrArr(struct __Pb__v__Pe___Type );
struct __Pb__i__Pe___Type __Pb__i__Pe___Type_Ret_Assign__Pb__i__Pe___Type_Arg___Pb__i__Pe___Type_Arg_(unsigned long long ,struct __Pb__i__Pe___Type );
struct __Pb__i__Pe___Type __Pb__i__Pe___Type_Ret_Increment__Pb__i__Pe___Type_Arg_(unsigned long long );
struct __Pb__Derived2__Pe___Type __Pb__Derived2__Pe___Type_Ret_Assign__Pb__Derived2__Pe___Type_Arg___Pb__Derived2__Pe___Type_Arg_(unsigned long long ,struct __Pb__Derived2__Pe___Type );
struct __Pb__f__Pe___Type __Pb__f__Pe___Type_Ret_Cast__Pb__i__Pe___Type_Arg__PntrArr(struct __Pb__i__Pe___Type );
struct __Pb__i__Pe___Type create_struct___Pb__i__Pe___Arg__UL_Arg____Pb__i__Pe___Type_Ret_(int *,unsigned long long );
struct __Pb__Derived2__Pe___Type __Pb__Derived2__Pe___Type_Ret_Cast__Pb__Base__Pe___Type_Arg__PntrArr(struct __Pb__Base__Pe___Type );
struct __Pb__Derived2__Pe___Type create_struct___Pb__Derived2__Pe___Arg__UL_Arg____Pb__Derived2__Pe___Type_Ret_(class Derived2 *,unsigned long long );
struct __Pb__Derived2__Pe___Type Assign(struct __Pb__Derived2__Pe___Type *,class Derived2 *);
class Derived1 *Deref___Pb__Derived1__Pe___Type_Arg____Pb__Derived1__Pe___Ret_(struct __Pb__Derived1__Pe___Type );
struct __Pb__Derived1__Pe___Type __Pb__Derived1__Pe___Type_Ret_Cast__Pb__Base__Pe___Type_Arg__PntrArr(struct __Pb__Base__Pe___Type );
struct __Pb__Derived1__Pe___Type AddressOf(unsigned long long ,unsigned int );
struct __Pb__Base__Pe___Type Assign(struct __Pb__Base__Pe___Type *,struct __Pb__Base__Pe___Type );
struct __Pb__Base__Pe___Type create_struct___Pb__Base__Pe___Arg__UL_Arg____Pb__Base__Pe___Type_Ret_(class Base *,unsigned long long );
struct __Pb__Base__Pe___Type Assign(struct __Pb__Base__Pe___Type *,class Base *);
struct __Pb__i__Pe___Type Assign(struct __Pb__i__Pe___Type *,struct __Pb__i__Pe___Type );
struct __Pb__i__Pe___Type __Pb__i__Pe___Type_Ret_Cast__Pb__v__Pe___Type_Arg__PntrArr(struct __Pb__v__Pe___Type );
struct __Pb__v__Pe___Type malloc_overload(unsigned long );
struct __Pb__Base__Pe___Type __Pb__Base__Pe___Type_Ret_DotExpnode_Arg___Pb__Base__Pe___Arg_(class Base *,unsigned long long );
struct node *PntrArrRef___Pb__node__Pe___Type_Arg__Ui_Arg____Pb__node__Pe___Ret_(struct __Pb__node__Pe___Type ,unsigned int );
bool b_Ret_NotEqual__Pb__node__Pe___Type_Arg___Pb__node__Pe___Type_Arg_(struct __Pb__node__Pe___Type ,struct __Pb__node__Pe___Type );
class Base *Deref___Pb__Base__Pe___Type_Arg____Pb__Base__Pe___Ret_(struct __Pb__Base__Pe___Type );
struct __Pb__node__Pe___Type __Pb__node__Pe___Type_Ret_Add__Pb__node__Pe___Type_Arg_Ui_Arg_(struct __Pb__node__Pe___Type ,unsigned int );
struct __Pb__v__Pe___Type __Pb__v__Pe___Type_Ret_Cast__Pb__node__Pe___Type_Arg__PntrArr(struct __Pb__node__Pe___Type );
void free_overload(struct __Pb__v__Pe___Type );
struct __Pb__v__Pe___Type __Pb__v__Pe___Type_Ret_Cast__Pb__Base__Pe___Type_Arg__PntrArr(struct __Pb__Base__Pe___Type );

struct __Pb__node__Pe___Type Assign(struct __Pb__node__Pe___Type *,struct __Pb__node__Pe___Type );
struct __Pb__node__Pe___Type __Pb__node__Pe___Type_Ret_Assign__Pb__node__Pe___Type_Arg___Pb__node__Pe___Type_Arg_(unsigned long long ,struct __Pb__node__Pe___Type );
struct __Pb__node__Pe___Type __Pb__node__Pe___Type_Ret_Casti_Arg__PntrArr_ValCast(int );
struct __Pb__node__Pe___Type __Pb__node__Pe___Type_Ret_ArrowExp__Pb__node__Pe___Arg___Pb__node__Pe___Arg_(struct node *,unsigned long long );
struct __Pb__Base__Pe___Type __Pb__Base__Pe___Type_Ret_Assign__Pb__Base__Pe___Type_Arg___Pb__Base__Pe___Type_Arg_(unsigned long long ,struct __Pb__Base__Pe___Type );
struct __Pb__Base__Pe___Type __Pb__Base__Pe___Type_Ret_ArrowExp__Pb__node__Pe___Arg___Pb__Base__Pe___Arg_(class Base *,unsigned long long );
struct node *Deref___Pb__node__Pe___Type_Arg____Pb__node__Pe___Ret_(struct __Pb__node__Pe___Type );
struct __Pb__node__Pe___Type create_struct___Pb__node__Pe___Arg__UL_Arg____Pb__node__Pe___Type_Ret_(struct node *,unsigned long long );
struct __Pb__node__Pe___Type Assign(struct __Pb__node__Pe___Type *,struct node *);

struct __Pb__Base__Pe___Type __Pb__Base__Pe___Type_Ret_Cast__Pb__Derived1__Pe___Type_Arg__PntrArr(struct __Pb__Derived1__Pe___Type );
struct __Pb__Derived1__Pe___Type __Pb__Derived1__Pe___Type_Ret_Cast__Pb__Derived2__Pe___Type_Arg__PntrArr(struct __Pb__Derived2__Pe___Type );
int *Deref___Pb__i__Pe___Type_Arg____Pb__i__Pe___Ret_(struct __Pb__i__Pe___Type );
class Derived2 *Deref___Pb__Derived2__Pe___Type_Arg____Pb__Derived2__Pe___Ret_(struct __Pb__Derived2__Pe___Type );

void create_entry(unsigned long long ,unsigned long long );


