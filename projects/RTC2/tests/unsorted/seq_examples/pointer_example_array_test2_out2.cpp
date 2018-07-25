#if 1
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
class node ;
class node ;

class node 
{
  public: class node *prev;
  class node *next;
  unsigned int val;
  unsigned int array1[10UL];
  

  inline void set_val(unsigned int input)
{
    (this) -> val = input;
  }
  

  inline unsigned int get_val()
{
    return (this) -> val;
  }
}
;
#endif

struct __Pb__i__Pe___Type 
{
  int *ptr;
  unsigned long long addr;
}
;
void v_Ret_create_entry_UL_Arg_UL_Arg(unsigned long long ,unsigned long long );

struct __Pb__si__Pe___Type 
{
  signed char *ptr;
  unsigned long long addr;
}
;
static struct __Pb__i__Pe___Type __Pb__i__Pe___Type_Ret_create_struct_from_addr_UL_Arg(unsigned long long );

static struct __Pb__i__Pe___Type __Pb__i__Pe___Type_Ret_create_struct_from_addr_UL_Arg(unsigned long long input1)
{
  struct __Pb__i__Pe___Type output;
  output.__Pb__i__Pe___Type::ptr =  *((int **)input1);
  output.__Pb__i__Pe___Type::addr = input1;
  return output;
}
static struct __Pb__i__Pe___Type __Pb__i__Pe___Type_Ret_Cast_i_Arg(int );

static struct __Pb__i__Pe___Type __Pb__i__Pe___Type_Ret_Cast_i_Arg(int input1)
{
  struct __Pb__i__Pe___Type output;
  output.__Pb__i__Pe___Type::ptr = ((int *)input1);
  output.__Pb__i__Pe___Type::addr = 0;
  return output;
}

struct __Pb__i__Pe___Type fn1(struct __Pb__i__Pe___Type int_array_str,struct __Pb__si__Pe___Type char_array_str)
{
  (v_Ret_create_entry_UL_Arg_UL_Arg(((unsigned long long )(&char_array_str.__Pb__si__Pe___Type::ptr)),char_array_str.__Pb__si__Pe___Type::addr) , char_array_str.__Pb__si__Pe___Type::addr = ((unsigned long long )(&char_array_str.__Pb__si__Pe___Type::ptr)));
  (v_Ret_create_entry_UL_Arg_UL_Arg(((unsigned long long )(&int_array_str.__Pb__i__Pe___Type::ptr)),int_array_str.__Pb__i__Pe___Type::addr) , int_array_str.__Pb__i__Pe___Type::addr = ((unsigned long long )(&int_array_str.__Pb__i__Pe___Type::ptr)));
  unsigned int counter = 0U;
  unsigned int index1 = 0U;
  printf("fn1: Printing int_array\n");
  for (; counter < index1; counter++) {
    unsigned int counter2 = 0U;
    for (; counter2 < 3U; counter2++) {
      printf("%d\n",int_array[counter][counter2]);
    }
  }
  printf("fn1: Done\n");
  index1 = 8U;
  printf("fn1: Printing char_array\n");
  char char_counter = '\0';
  for (; char_counter < index1; char_counter++) {
    printf("%c\n",char_array[char_counter]);
  }
  printf("fn1: Done\n");
  return __Pb__i__Pe___Type_Ret_Cast_i_Arg(0);
}

struct __Pb__v__Pe___Type 
{
  void *ptr;
  unsigned long long addr;
}
;
struct __Pb__v__Pe___Type __Pb__v__Pe___Type_Ret_malloc_overload_Ul_Arg(unsigned long );

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
static struct __Pb__Ui__Pe___Type __Pb__Ui__Pe___Type_Ret_Assign___Pb____Pb__Ui__Pe___Type__Pe___Arg___Pb__Ui__Pe___Type_Arg(struct __Pb__Ui__Pe___Type *,struct __Pb__Ui__Pe___Type );

static struct __Pb__Ui__Pe___Type __Pb__Ui__Pe___Type_Ret_Assign___Pb____Pb__Ui__Pe___Type__Pe___Arg___Pb__Ui__Pe___Type_Arg(struct __Pb__Ui__Pe___Type *input1,struct __Pb__Ui__Pe___Type input2)
{
  input1 -> __Pb__Ui__Pe___Type::ptr = input2.__Pb__Ui__Pe___Type::ptr;
  v_Ret_create_entry_UL_Arg_UL_Arg(input1 -> __Pb__Ui__Pe___Type::addr,input2.__Pb__Ui__Pe___Type::addr);
  return  *input1;
}
static struct __Pb__Ui__Pe___Type __Pb__Ui__Pe___Type_Ret_create_struct___Pb__Ui__Pe___Arg_UL_Arg(unsigned int *,unsigned long long );

static struct __Pb__Ui__Pe___Type __Pb__Ui__Pe___Type_Ret_create_struct___Pb__Ui__Pe___Arg_UL_Arg(unsigned int *input1,unsigned long long input2)
{
  struct __Pb__Ui__Pe___Type output;
  output.__Pb__Ui__Pe___Type::ptr = input1;
  output.__Pb__Ui__Pe___Type::addr = input2;
  return output;
}
static struct __Pb__Ui__Pe___Type __Pb__Ui__Pe___Type_Ret_Add___Pb__Ui__Pe___Type_Arg_i_Arg(struct __Pb__Ui__Pe___Type ,int );

static struct __Pb__Ui__Pe___Type __Pb__Ui__Pe___Type_Ret_Add___Pb__Ui__Pe___Type_Arg_i_Arg(struct __Pb__Ui__Pe___Type input1,int input2)
{
  input1.__Pb__Ui__Pe___Type::ptr += input2;
  return input1;
}
static int i_Ret_LessThan___Pb__Ui__Pe___Type_Arg___Pb__Ui__Pe___Type_Arg(struct __Pb__Ui__Pe___Type ,struct __Pb__Ui__Pe___Type );

static int i_Ret_LessThan___Pb__Ui__Pe___Type_Arg___Pb__Ui__Pe___Type_Arg(struct __Pb__Ui__Pe___Type input1,struct __Pb__Ui__Pe___Type input2)
{
  return input1.__Pb__Ui__Pe___Type::ptr < input2.__Pb__Ui__Pe___Type::ptr;
}
static struct __Pb__Ui__Pe___Type __Pb__Ui__Pe___Type_Ret_Increment___Pb____Pb__Ui__Pe___Type__Pe___Arg(struct __Pb__Ui__Pe___Type *);

static struct __Pb__Ui__Pe___Type __Pb__Ui__Pe___Type_Ret_Increment___Pb____Pb__Ui__Pe___Type__Pe___Arg(struct __Pb__Ui__Pe___Type *input1)
{
  ++input1 -> __Pb__Ui__Pe___Type::ptr;
  return  *input1;
}
static unsigned int *__Pb__Ui__Pe___Ret_Deref___Pb__Ui__Pe___Type_Arg(struct __Pb__Ui__Pe___Type );

static unsigned int *__Pb__Ui__Pe___Ret_Deref___Pb__Ui__Pe___Type_Arg(struct __Pb__Ui__Pe___Type input1)
{
  v_Ret_check_entry_UL_Arg_UL_Arg(((unsigned long long )input1.__Pb__Ui__Pe___Type::ptr),input1.__Pb__Ui__Pe___Type::addr);
  return input1.__Pb__Ui__Pe___Type::ptr;
}
void v_Ret_check_entry_UL_Arg_UL_Arg(unsigned long long ,unsigned long long );
static unsigned long Ul_Ret_Cast___Pb__Ui__Pe___Type_Arg(struct __Pb__Ui__Pe___Type );

static unsigned long Ul_Ret_Cast___Pb__Ui__Pe___Type_Arg(struct __Pb__Ui__Pe___Type input1)
{
  return (unsigned long long )input1.__Pb__Ui__Pe___Type::ptr;
}
static struct __Pb__i__Pe___Type __Pb__i__Pe___Type_Ret_create_struct___Pb__i__Pe___Arg_UL_Arg_Ui_Arg(int *,unsigned long long ,unsigned int );
void v_Ret_create_entry_UL_Arg_UL_Arg_Ul_Arg(unsigned long long ,unsigned long long ,unsigned long );

static struct __Pb__i__Pe___Type __Pb__i__Pe___Type_Ret_create_struct___Pb__i__Pe___Arg_UL_Arg_Ui_Arg(int *input1,unsigned long long input2,unsigned int input3)
{
  struct __Pb__i__Pe___Type output;
  output.__Pb__i__Pe___Type::ptr = input1;
  output.__Pb__i__Pe___Type::addr = input2;
  v_Ret_create_entry_UL_Arg_UL_Arg_Ul_Arg(input2,((unsigned long long )input1),((unsigned long )input3));
  return output;
}
static struct __Pb__i__Pe___Type __Pb__i__Pe___Type_Ret_Cast___Pb__i__Pe___Type_Arg(struct __Pb__i__Pe___Type );

static struct __Pb__i__Pe___Type __Pb__i__Pe___Type_Ret_Cast___Pb__i__Pe___Type_Arg(struct __Pb__i__Pe___Type input1)
{
  struct __Pb__i__Pe___Type output;
  output.__Pb__i__Pe___Type::ptr = ((int *)input1.__Pb__i__Pe___Type::ptr);
  output.__Pb__i__Pe___Type::addr = input1.__Pb__i__Pe___Type::addr;
  return output;
}
static struct __Pb__i__Pe___Type __Pb__i__Pe___Type_Ret_Assign___Pb____Pb__i__Pe___Type__Pe___Arg___Pb__i__Pe___Type_Arg(struct __Pb__i__Pe___Type *,struct __Pb__i__Pe___Type );

static struct __Pb__i__Pe___Type __Pb__i__Pe___Type_Ret_Assign___Pb____Pb__i__Pe___Type__Pe___Arg___Pb__i__Pe___Type_Arg(struct __Pb__i__Pe___Type *input1,struct __Pb__i__Pe___Type input2)
{
  input1 -> __Pb__i__Pe___Type::ptr = input2.__Pb__i__Pe___Type::ptr;
  v_Ret_create_entry_UL_Arg_UL_Arg(input1 -> __Pb__i__Pe___Type::addr,input2.__Pb__i__Pe___Type::addr);
  return  *input1;
}
static struct __Pb__si__Pe___Type __Pb__si__Pe___Type_Ret_create_struct___Pb__si__Pe___Arg_UL_Arg_Ui_Arg(signed char *,unsigned long long ,unsigned int );

static struct __Pb__si__Pe___Type __Pb__si__Pe___Type_Ret_create_struct___Pb__si__Pe___Arg_UL_Arg_Ui_Arg(signed char *input1,unsigned long long input2,unsigned int input3)
{
  struct __Pb__si__Pe___Type output;
  output.__Pb__si__Pe___Type::ptr = input1;
  output.__Pb__si__Pe___Type::addr = input2;
  v_Ret_create_entry_UL_Arg_UL_Arg_Ul_Arg(input2,((unsigned long long )input1),((unsigned long )input3));
  return output;
}

struct __Pb____Pb__i__Pe____Pe___Type 
{
  int **ptr;
  unsigned long long addr;
}
;
static void v_Ret_AddressOf___Pb____Pb____Pb__i__Pe____Pe___Type__Pe___Arg___Pb__i__Pe___Type_Arg(struct __Pb____Pb__i__Pe____Pe___Type *,struct __Pb__i__Pe___Type );

static void v_Ret_AddressOf___Pb____Pb____Pb__i__Pe____Pe___Type__Pe___Arg___Pb__i__Pe___Type_Arg(struct __Pb____Pb__i__Pe____Pe___Type *input1,struct __Pb__i__Pe___Type input2)
{
  input1 -> __Pb____Pb__i__Pe____Pe___Type::ptr = ((int **)input2.__Pb__i__Pe___Type::addr);
  input1 -> __Pb____Pb__i__Pe____Pe___Type::addr = ((unsigned long long )(&input1 -> __Pb____Pb__i__Pe____Pe___Type::ptr));
  v_Ret_create_entry_UL_Arg_UL_Arg_Ul_Arg(input1 -> __Pb____Pb__i__Pe____Pe___Type::addr,((unsigned long long )(input1 -> __Pb____Pb__i__Pe____Pe___Type::ptr)),((unsigned long )(sizeof(input2.__Pb__i__Pe___Type::addr))));
}
static struct __Pb__i__Pe___Type __Pb__i__Pe___Type_Ret_Cast___Pb____Pb__i__Pe____Pe___Type_Arg(struct __Pb____Pb__i__Pe____Pe___Type );

static struct __Pb__i__Pe___Type __Pb__i__Pe___Type_Ret_Cast___Pb____Pb__i__Pe____Pe___Type_Arg(struct __Pb____Pb__i__Pe____Pe___Type input1)
{
  struct __Pb__i__Pe___Type output;
  output.__Pb__i__Pe___Type::ptr = ((int *)input1.__Pb____Pb__i__Pe____Pe___Type::ptr);
  output.__Pb__i__Pe___Type::addr = input1.__Pb____Pb__i__Pe____Pe___Type::addr;
  return output;
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

struct __Pb____Pb__si__Pe____Pe___Type 
{
  signed char **ptr;
  unsigned long long addr;
}
;
static void v_Ret_AddressOf___Pb____Pb____Pb__si__Pe____Pe___Type__Pe___Arg___Pb__si__Pe___Type_Arg(struct __Pb____Pb__si__Pe____Pe___Type *,struct __Pb__si__Pe___Type );

static void v_Ret_AddressOf___Pb____Pb____Pb__si__Pe____Pe___Type__Pe___Arg___Pb__si__Pe___Type_Arg(struct __Pb____Pb__si__Pe____Pe___Type *input1,struct __Pb__si__Pe___Type input2)
{
  input1 -> __Pb____Pb__si__Pe____Pe___Type::ptr = ((signed char **)input2.__Pb__si__Pe___Type::addr);
  input1 -> __Pb____Pb__si__Pe____Pe___Type::addr = ((unsigned long long )(&input1 -> __Pb____Pb__si__Pe____Pe___Type::ptr));
  v_Ret_create_entry_UL_Arg_UL_Arg_Ul_Arg(input1 -> __Pb____Pb__si__Pe____Pe___Type::addr,((unsigned long long )(input1 -> __Pb____Pb__si__Pe____Pe___Type::ptr)),((unsigned long )(sizeof(input2.__Pb__si__Pe___Type::addr))));
}
static struct __Pb__si__Pe___Type __Pb__si__Pe___Type_Ret_Cast___Pb____Pb__si__Pe____Pe___Type_Arg(struct __Pb____Pb__si__Pe____Pe___Type );

static struct __Pb__si__Pe___Type __Pb__si__Pe___Type_Ret_Cast___Pb____Pb__si__Pe____Pe___Type_Arg(struct __Pb____Pb__si__Pe____Pe___Type input1)
{
  struct __Pb__si__Pe___Type output;
  output.__Pb__si__Pe___Type::ptr = ((signed char *)input1.__Pb____Pb__si__Pe____Pe___Type::ptr);
  output.__Pb__si__Pe___Type::addr = input1.__Pb____Pb__si__Pe____Pe___Type::addr;
  return output;
}
static struct __Pb__si__Pe___Type __Pb__si__Pe___Type_Ret_Assign___Pb____Pb__si__Pe___Type__Pe___Arg___Pb__si__Pe___Type_Arg(struct __Pb__si__Pe___Type *,struct __Pb__si__Pe___Type );

static struct __Pb__si__Pe___Type __Pb__si__Pe___Type_Ret_Assign___Pb____Pb__si__Pe___Type__Pe___Arg___Pb__si__Pe___Type_Arg(struct __Pb__si__Pe___Type *input1,struct __Pb__si__Pe___Type input2)
{
  input1 -> __Pb__si__Pe___Type::ptr = input2.__Pb__si__Pe___Type::ptr;
  v_Ret_create_entry_UL_Arg_UL_Arg(input1 -> __Pb__si__Pe___Type::addr,input2.__Pb__si__Pe___Type::addr);
  return  *input1;
}
static struct __Pb__si__Pe___Type __Pb__si__Pe___Type_Ret_create_struct___Pb__si__Pe___Arg_UL_Arg(signed char *,unsigned long long );

static struct __Pb__si__Pe___Type __Pb__si__Pe___Type_Ret_create_struct___Pb__si__Pe___Arg_UL_Arg(signed char *input1,unsigned long long input2)
{
  struct __Pb__si__Pe___Type output;
  output.__Pb__si__Pe___Type::ptr = input1;
  output.__Pb__si__Pe___Type::addr = input2;
  return output;
}
static signed char *__Pb__si__Pe___Ret_Deref___Pb__si__Pe___Type_Arg(struct __Pb__si__Pe___Type );

static signed char *__Pb__si__Pe___Ret_Deref___Pb__si__Pe___Type_Arg(struct __Pb__si__Pe___Type input1)
{
  v_Ret_check_entry_UL_Arg_UL_Arg(((unsigned long long )input1.__Pb__si__Pe___Type::ptr),input1.__Pb__si__Pe___Type::addr);
  return input1.__Pb__si__Pe___Type::ptr;
}
static struct __Pb__i__Pe___Type __Pb__i__Pe___Type_Ret_Add___Pb__i__Pe___Type_Arg_UL_Arg(struct __Pb__i__Pe___Type ,unsigned long long );

static struct __Pb__i__Pe___Type __Pb__i__Pe___Type_Ret_Add___Pb__i__Pe___Type_Arg_UL_Arg(struct __Pb__i__Pe___Type input1,unsigned long long input2)
{
  input1.__Pb__i__Pe___Type::ptr += input2;
  return input1;
}
static struct __Pb__Ui__Pe___Type __Pb__Ui__Pe___Type_Ret_Sub___Pb__Ui__Pe___Type_Arg_i_Arg(struct __Pb__Ui__Pe___Type ,int );

static struct __Pb__Ui__Pe___Type __Pb__Ui__Pe___Type_Ret_Sub___Pb__Ui__Pe___Type_Arg_i_Arg(struct __Pb__Ui__Pe___Type input1,int input2)
{
  input1.__Pb__Ui__Pe___Type::ptr -= input2;
  return input1;
}
static int i_Ret_GreaterOrEqual___Pb__Ui__Pe___Type_Arg___Pb__Ui__Pe___Type_Arg(struct __Pb__Ui__Pe___Type ,struct __Pb__Ui__Pe___Type );

static int i_Ret_GreaterOrEqual___Pb__Ui__Pe___Type_Arg___Pb__Ui__Pe___Type_Arg(struct __Pb__Ui__Pe___Type input1,struct __Pb__Ui__Pe___Type input2)
{
  return input1.__Pb__Ui__Pe___Type::ptr >= input2.__Pb__Ui__Pe___Type::ptr;
}
static struct __Pb__Ui__Pe___Type __Pb__Ui__Pe___Type_Ret_Decrement___Pb____Pb__Ui__Pe___Type__Pe___Arg(struct __Pb__Ui__Pe___Type *);

static struct __Pb__Ui__Pe___Type __Pb__Ui__Pe___Type_Ret_Decrement___Pb____Pb__Ui__Pe___Type__Pe___Arg(struct __Pb__Ui__Pe___Type *input1)
{
  --input1 -> __Pb__Ui__Pe___Type::ptr;
  return  *input1;
}

struct __Pb__node__Pe___Type 
{
  class node *ptr;
  unsigned long long addr;
}
;
static struct __Pb__node__Pe___Type __Pb__node__Pe___Type_Ret_create_struct___Pb__node__Pe___Arg_UL_Arg_Ui_Arg(class node *,unsigned long long ,unsigned int );

static struct __Pb__node__Pe___Type __Pb__node__Pe___Type_Ret_create_struct___Pb__node__Pe___Arg_UL_Arg_Ui_Arg(class node *input1,unsigned long long input2,unsigned int input3)
{
  struct __Pb__node__Pe___Type output;
  output.__Pb__node__Pe___Type::ptr = input1;
  output.__Pb__node__Pe___Type::addr = input2;
  v_Ret_create_entry_UL_Arg_UL_Arg_Ul_Arg(input2,((unsigned long long )input1),((unsigned long )input3));
  return output;
}
static struct __Pb____Pb__i__Pe____Pe___Type __Pb____Pb__i__Pe____Pe___Type_Ret_create_struct___Pb____Pb__i__Pe____Pe___Arg_UL_Arg_Ui_Arg(int **,unsigned long long ,unsigned int );

static struct __Pb____Pb__i__Pe____Pe___Type __Pb____Pb__i__Pe____Pe___Type_Ret_create_struct___Pb____Pb__i__Pe____Pe___Arg_UL_Arg_Ui_Arg(int **input1,unsigned long long input2,unsigned int input3)
{
  struct __Pb____Pb__i__Pe____Pe___Type output;
  output.__Pb____Pb__i__Pe____Pe___Type::ptr = input1;
  output.__Pb____Pb__i__Pe____Pe___Type::addr = input2;
  v_Ret_create_entry_UL_Arg_UL_Arg_Ul_Arg(input2,((unsigned long long )input1),((unsigned long )input3));
  return output;
}
static struct __Pb__i__Pe___Type __Pb__i__Pe___Type_Ret_Cast___Pb__v__Pe___Type_Arg(struct __Pb__v__Pe___Type );

static struct __Pb__i__Pe___Type __Pb__i__Pe___Type_Ret_Cast___Pb__v__Pe___Type_Arg(struct __Pb__v__Pe___Type input1)
{
  struct __Pb__i__Pe___Type output;
  output.__Pb__i__Pe___Type::ptr = ((int *)input1.__Pb__v__Pe___Type::ptr);
  output.__Pb__i__Pe___Type::addr = input1.__Pb__v__Pe___Type::addr;
  return output;
}

int main()
{
    #if 1
  unsigned int *no_init_ptr;
  printf("before ptr\n");
  unsigned int *ptr;
  struct __Pb__Ui__Pe___Type ptr_str;
  ((ptr_str.__Pb__Ui__Pe___Type::ptr = ptr , ptr_str.__Pb__Ui__Pe___Type::addr = ((unsigned long long )(&ptr))) , ((__Pb__Ui__Pe___Type_Ret_Assign___Pb____Pb__Ui__Pe___Type__Pe___Arg___Pb__Ui__Pe___Type_Arg(&ptr_str,__Pb__Ui__Pe___Type_Ret_Cast___Pb__v__Pe___Type_Arg(__Pb__v__Pe___Type_Ret_malloc_overload_Ul_Arg(((unsigned long )400UL)))) , ptr = ptr_str.__Pb__Ui__Pe___Type::ptr)));
  printf("before ptr2\n");
  unsigned int *ptr2;
  struct __Pb__Ui__Pe___Type ptr2_str;
  ((ptr2_str.__Pb__Ui__Pe___Type::ptr = ptr2 , ptr2_str.__Pb__Ui__Pe___Type::addr = ((unsigned long long )(&ptr2))) , ((__Pb__Ui__Pe___Type_Ret_Assign___Pb____Pb__Ui__Pe___Type__Pe___Arg___Pb__Ui__Pe___Type_Arg(&ptr2_str,__Pb__Ui__Pe___Type_Ret_Cast___Pb__v__Pe___Type_Arg(__Pb__v__Pe___Type_Ret_malloc_overload_Ul_Arg(((unsigned long )40UL)))) , ptr2 = ptr2_str.__Pb__Ui__Pe___Type::ptr)));
  printf("before ptr_index\n");
  unsigned int *ptr_index;
  struct __Pb__Ui__Pe___Type ptr_index_str;
  ((ptr_index_str.__Pb__Ui__Pe___Type::ptr = ptr_index , ptr_index_str.__Pb__Ui__Pe___Type::addr = ((unsigned long long )(&ptr_index))) , ((__Pb__Ui__Pe___Type_Ret_Assign___Pb____Pb__Ui__Pe___Type__Pe___Arg___Pb__Ui__Pe___Type_Arg(&ptr_index_str,__Pb__Ui__Pe___Type_Ret_create_struct___Pb__Ui__Pe___Arg_UL_Arg(ptr,((unsigned long long )(&ptr)))) , ptr_index = ptr_index_str.__Pb__Ui__Pe___Type::ptr)));
  unsigned int counter = 0U;
  printf("before loop1\n");
  struct __Pb__Ui__Pe___Type __Pb__Ui__Pe___Type_Var_ovl_0;
  for (; i_Ret_LessThan___Pb__Ui__Pe___Type_Arg___Pb__Ui__Pe___Type_Arg(__Pb__Ui__Pe___Type_Ret_create_struct___Pb__Ui__Pe___Arg_UL_Arg(ptr_index,((unsigned long long )(&ptr_index))),__Pb__Ui__Pe___Type_Ret_Add___Pb__Ui__Pe___Type_Arg_i_Arg(__Pb__Ui__Pe___Type_Ret_create_struct___Pb__Ui__Pe___Arg_UL_Arg(ptr,((unsigned long long )(&ptr))),100)); (((__Pb__Ui__Pe___Type_Var_ovl_0 = __Pb__Ui__Pe___Type_Ret_create_struct___Pb__Ui__Pe___Arg_UL_Arg(ptr_index,((unsigned long long )(&ptr_index))) , __Pb__Ui__Pe___Type_Ret_Increment___Pb____Pb__Ui__Pe___Type__Pe___Arg(&__Pb__Ui__Pe___Type_Var_ovl_0)) ,  *((unsigned int **)__Pb__Ui__Pe___Type_Var_ovl_0.__Pb__Ui__Pe___Type::addr) = __Pb__Ui__Pe___Type_Var_ovl_0.__Pb__Ui__Pe___Type::ptr) , __Pb__Ui__Pe___Type_Var_ovl_0)) {
     *__Pb__Ui__Pe___Ret_Deref___Pb__Ui__Pe___Type_Arg(__Pb__Ui__Pe___Type_Ret_create_struct___Pb__Ui__Pe___Arg_UL_Arg(ptr_index,((unsigned long long )(&ptr_index)))) = counter++;
    printf("%lu\n",Ul_Ret_Cast___Pb__Ui__Pe___Type_Arg(__Pb__Ui__Pe___Type_Ret_create_struct___Pb__Ui__Pe___Arg_UL_Arg(ptr_index,((unsigned long long )(&ptr_index)))));
  }
    #else
    #endif
  int int_array_complex[] = {(1), (1), (2), (3), (5), (8), (13), (21)};
  signed char schar_array_complex[] = "abc";
  int myPoints[][3UL] = {{(1), (2), (3)}, {(4), (5), (6)}, {(7), (8), (9)}};
  int arr3d[2UL][3UL][4UL] = {{{(1), (2), (3), (4)}, {(1), (2), (3), (4)}, {(1), (2), (3), (4)}}, {{(1), (2), (3), (4)}, {(1), (2), (3), (4)}, {(1), (2), (3), (4)}}};
  int *myPoints_ptr;
  struct __Pb__i__Pe___Type myPoints_ptr_str;
  ((myPoints_ptr_str.__Pb__i__Pe___Type::ptr = myPoints_ptr , myPoints_ptr_str.__Pb__i__Pe___Type::addr = ((unsigned long long )(&myPoints_ptr))) , ((__Pb__i__Pe___Type_Ret_Assign___Pb____Pb__i__Pe___Type__Pe___Arg___Pb__i__Pe___Type_Arg(&myPoints_ptr_str,__Pb__i__Pe___Type_Ret_Cast___Pb__i__Pe___Type_Arg(__Pb__i__Pe___Type_Ret_create_struct___Pb__i__Pe___Arg_UL_Arg_Ui_Arg(((int *)myPoints),((unsigned long long )(&myPoints)),sizeof(myPoints)))) , myPoints_ptr = myPoints_ptr_str.__Pb__i__Pe___Type::ptr)));
  ::fn1(__Pb__i__Pe___Type_Ret_create_struct___Pb__i__Pe___Arg_UL_Arg_Ui_Arg(((int *)myPoints),((unsigned long long )(&myPoints)),sizeof(myPoints)),__Pb__si__Pe___Type_Ret_create_struct___Pb__si__Pe___Arg_UL_Arg_Ui_Arg(((signed char *)schar_array_complex),((unsigned long long )(&schar_array_complex)),sizeof(schar_array_complex)));
  struct __Pb____Pb__i__Pe____Pe___Type __Pb__i__Pe___Type_Var_addr_1;
  int *int_array_complex_ptr;
  struct __Pb__i__Pe___Type int_array_complex_ptr_str;
  ((int_array_complex_ptr_str.__Pb__i__Pe___Type::ptr = int_array_complex_ptr , int_array_complex_ptr_str.__Pb__i__Pe___Type::addr = ((unsigned long long )(&int_array_complex_ptr))) , ((__Pb__i__Pe___Type_Ret_Assign___Pb____Pb__i__Pe___Type__Pe___Arg___Pb__i__Pe___Type_Arg(&int_array_complex_ptr_str,__Pb__i__Pe___Type_Ret_Cast___Pb____Pb__i__Pe____Pe___Type_Arg(((v_Ret_AddressOf___Pb____Pb____Pb__i__Pe____Pe___Type__Pe___Arg___Pb__i__Pe___Type_Arg(&__Pb__i__Pe___Type_Var_addr_1,__Pb__i__Pe___Type_Ret_create_struct___Pb__i__Pe___Arg_UL_Arg_Ui_Arg(((int *)int_array_complex),((unsigned long long )(&int_array_complex)),sizeof(int_array_complex))) , __Pb__i__Pe___Type_Var_addr_1)))) , int_array_complex_ptr = int_array_complex_ptr_str.__Pb__i__Pe___Type::ptr)));
  printf("int_array_complex begin\n");
  for (counter = 0U; counter < 8ULL; counter++) {
    printf("%d\n",int_array_complex[counter]);
    printf("check: %d\n",__Pb__i__Pe___Ret_Deref___Pb__i__Pe___Type_Arg(__Pb__i__Pe___Type_Ret_create_struct___Pb__i__Pe___Arg_UL_Arg(int_array_complex_ptr,((unsigned long long )(&int_array_complex_ptr))))[counter * 1ULL]);
  }
  printf("int_array_complex end\n");
  struct __Pb____Pb__si__Pe____Pe___Type __Pb__si__Pe___Type_Var_addr_2;
  signed char *schar_array_complex_ptr;
  struct __Pb__si__Pe___Type schar_array_complex_ptr_str;
  ((schar_array_complex_ptr_str.__Pb__si__Pe___Type::ptr = schar_array_complex_ptr , schar_array_complex_ptr_str.__Pb__si__Pe___Type::addr = ((unsigned long long )(&schar_array_complex_ptr))) , ((__Pb__si__Pe___Type_Ret_Assign___Pb____Pb__si__Pe___Type__Pe___Arg___Pb__si__Pe___Type_Arg(&schar_array_complex_ptr_str,__Pb__si__Pe___Type_Ret_Cast___Pb____Pb__si__Pe____Pe___Type_Arg(((v_Ret_AddressOf___Pb____Pb____Pb__si__Pe____Pe___Type__Pe___Arg___Pb__si__Pe___Type_Arg(&__Pb__si__Pe___Type_Var_addr_2,__Pb__si__Pe___Type_Ret_create_struct___Pb__si__Pe___Arg_UL_Arg_Ui_Arg(((signed char *)schar_array_complex),((unsigned long long )(&schar_array_complex)),sizeof(schar_array_complex))) , __Pb__si__Pe___Type_Var_addr_2)))) , schar_array_complex_ptr = schar_array_complex_ptr_str.__Pb__si__Pe___Type::ptr)));
  printf("schar_array_complex begin\n");
  for (counter = 0U; counter < 4ULL; counter++) {
    printf("%c\n",schar_array_complex[counter]);
    printf("check: %c\n",__Pb__si__Pe___Ret_Deref___Pb__si__Pe___Type_Arg(__Pb__si__Pe___Type_Ret_create_struct___Pb__si__Pe___Arg_UL_Arg(schar_array_complex_ptr,((unsigned long long )(&schar_array_complex_ptr))))[counter * 1ULL]);
  }
  printf("schar_array_complex end\n");
    #if 1
  printf("myPoints begin\n");
  for (counter = 0U; counter < 3ULL; counter++) {
    int counter2;
    for (counter2 = 0; counter2 < 3ULL; counter2++) {
      printf("%d\n",myPoints[counter][counter2]);
            #if 1
      printf("check: %d\n",__Pb__i__Pe___Ret_Deref___Pb__i__Pe___Type_Arg(__Pb__i__Pe___Type_Ret_Add___Pb__i__Pe___Type_Arg_UL_Arg(__Pb__i__Pe___Type_Ret_create_struct___Pb__i__Pe___Arg_UL_Arg(myPoints_ptr,((unsigned long long )(&myPoints_ptr))),(counter * 3ULL)))[counter2 * 1ULL]);
            #endif
    }
  }
  printf("myPoints end\n");
    #endif
    #if 1
  int *arr3d_ptr;
  struct __Pb__i__Pe___Type arr3d_ptr_str;
  ((arr3d_ptr_str.__Pb__i__Pe___Type::ptr = arr3d_ptr , arr3d_ptr_str.__Pb__i__Pe___Type::addr = ((unsigned long long )(&arr3d_ptr))) , ((__Pb__i__Pe___Type_Ret_Assign___Pb____Pb__i__Pe___Type__Pe___Arg___Pb__i__Pe___Type_Arg(&arr3d_ptr_str,__Pb__i__Pe___Type_Ret_Cast___Pb__i__Pe___Type_Arg(__Pb__i__Pe___Type_Ret_create_struct___Pb__i__Pe___Arg_UL_Arg_Ui_Arg(((int *)arr3d),((unsigned long long )(&arr3d)),sizeof(arr3d)))) , arr3d_ptr = arr3d_ptr_str.__Pb__i__Pe___Type::ptr)));
  printf("arr3d begin\n");
  for (counter = 0U; counter < 2ULL; counter++) {
    int counter2;
    for (counter2 = 0; counter2 < 3ULL; counter2++) {
      int counter3;
      for (counter3 = 0; counter3 < 4ULL; counter3++) {
        printf("%d\n",arr3d[counter][counter2][counter3]);
// TESTING
                #if 0
                #endif
                #if 1
        printf("check: %d\n",__Pb__i__Pe___Ret_Deref___Pb__i__Pe___Type_Arg(__Pb__i__Pe___Type_Ret_Add___Pb__i__Pe___Type_Arg_UL_Arg(__Pb__i__Pe___Type_Ret_Add___Pb__i__Pe___Type_Arg_UL_Arg(__Pb__i__Pe___Type_Ret_create_struct___Pb__i__Pe___Arg_UL_Arg(arr3d_ptr,((unsigned long long )(&arr3d_ptr))),(counter * 12ULL)),(counter2 * 4ULL)))[counter3 * 1ULL]);
                #endif
      }
    }
  }
  printf("arr3d end\n");
    #endif
    #if 1
    #if 1
  int array[100UL];
  printf("before loop2\n");
  for (counter = 0U; counter < 100U; counter++) {
    array[counter] = counter;
  }
  printf("%u\n", *__Pb__i__Pe___Ret_Deref___Pb__i__Pe___Type_Arg(__Pb__i__Pe___Type_Ret_create_struct___Pb__i__Pe___Arg_UL_Arg_Ui_Arg(((int *)array),((unsigned long long )(&array)),sizeof(array))));
  printf("%u\n",array[0]);
  printf("%u\n", *__Pb__i__Pe___Ret_Deref___Pb__i__Pe___Type_Arg(__Pb__i__Pe___Type_Ret_create_struct___Pb__i__Pe___Arg_UL_Arg_Ui_Arg(((int *)array),((unsigned long long )(&array)),sizeof(array))));
  printf("Done\n");
    #endif
  printf("before loop3\n");
  struct __Pb__Ui__Pe___Type __Pb__Ui__Pe___Type_Var_ovl_3;
  struct __Pb__Ui__Pe___Type __Pb__Ui__Pe___Type_Var_ovl_4;
  for ((((__Pb__Ui__Pe___Type_Var_ovl_3 = __Pb__Ui__Pe___Type_Ret_create_struct___Pb__Ui__Pe___Arg_UL_Arg(ptr_index,((unsigned long long )(&ptr_index))) , __Pb__Ui__Pe___Type_Ret_Assign___Pb____Pb__Ui__Pe___Type__Pe___Arg___Pb__Ui__Pe___Type_Arg(&__Pb__Ui__Pe___Type_Var_ovl_3,__Pb__Ui__Pe___Type_Ret_Sub___Pb__Ui__Pe___Type_Arg_i_Arg(__Pb__Ui__Pe___Type_Ret_Add___Pb__Ui__Pe___Type_Arg_i_Arg(__Pb__Ui__Pe___Type_Ret_create_struct___Pb__Ui__Pe___Arg_UL_Arg(ptr,((unsigned long long )(&ptr))),100),1))) ,  *((unsigned int **)__Pb__Ui__Pe___Type_Var_ovl_3.__Pb__Ui__Pe___Type::addr) = __Pb__Ui__Pe___Type_Var_ovl_3.__Pb__Ui__Pe___Type::ptr) , __Pb__Ui__Pe___Type_Var_ovl_3); i_Ret_GreaterOrEqual___Pb__Ui__Pe___Type_Arg___Pb__Ui__Pe___Type_Arg(__Pb__Ui__Pe___Type_Ret_create_struct___Pb__Ui__Pe___Arg_UL_Arg(ptr_index,((unsigned long long )(&ptr_index))),__Pb__Ui__Pe___Type_Ret_create_struct___Pb__Ui__Pe___Arg_UL_Arg(ptr,((unsigned long long )(&ptr)))); (((__Pb__Ui__Pe___Type_Var_ovl_4 = __Pb__Ui__Pe___Type_Ret_create_struct___Pb__Ui__Pe___Arg_UL_Arg(ptr_index,((unsigned long long )(&ptr_index))) , __Pb__Ui__Pe___Type_Ret_Decrement___Pb____Pb__Ui__Pe___Type__Pe___Arg(&__Pb__Ui__Pe___Type_Var_ovl_4)) ,  *((unsigned int **)__Pb__Ui__Pe___Type_Var_ovl_4.__Pb__Ui__Pe___Type::addr) = __Pb__Ui__Pe___Type_Var_ovl_4.__Pb__Ui__Pe___Type::ptr) , __Pb__Ui__Pe___Type_Var_ovl_4)) {
    printf("%lu\n",Ul_Ret_Cast___Pb__Ui__Pe___Type_Arg(__Pb__Ui__Pe___Type_Ret_create_struct___Pb__Ui__Pe___Arg_UL_Arg(ptr,((unsigned long long )(&ptr)))));
    printf("%lu\n",Ul_Ret_Cast___Pb__Ui__Pe___Type_Arg(__Pb__Ui__Pe___Type_Ret_create_struct___Pb__Ui__Pe___Arg_UL_Arg(ptr_index,((unsigned long long )(&ptr_index)))));
    printf("%u\n", *__Pb__Ui__Pe___Ret_Deref___Pb__Ui__Pe___Type_Arg(__Pb__Ui__Pe___Type_Ret_create_struct___Pb__Ui__Pe___Arg_UL_Arg(ptr_index,((unsigned long long )(&ptr_index)))));
  }
    #if 1
  printf("before loop4\n");
  class node class_array[100UL];
  for (counter = 0U; counter < 100U; counter++) {
    class_array[counter]. set_val (counter);
  }
  printf("Printing class array\n");
  printf("before loop5\n");
  for (counter = 0U; counter < 100U; counter++) {
    printf("%u\n",class_array[counter]. get_val ());
  }
    #endif
    #if 1
  int *array_ptr[100UL];
  printf("before loop6\n");
  for (counter = 0U; counter < 100U; counter++) {
    struct __Pb__i__Pe___Type __Pb__i__Pe___Type_Var_ovl_5;
    (((__Pb__i__Pe___Type_Var_ovl_5 = __Pb__i__Pe___Type_Ret_create_struct_from_addr_UL_Arg(((unsigned long long )(&array_ptr[counter]))) , __Pb__i__Pe___Type_Ret_Assign___Pb____Pb__i__Pe___Type__Pe___Arg___Pb__i__Pe___Type_Arg(&__Pb__i__Pe___Type_Var_ovl_5,__Pb__i__Pe___Type_Ret_Cast___Pb__v__Pe___Type_Arg(__Pb__v__Pe___Type_Ret_malloc_overload_Ul_Arg(((unsigned long )4UL))))) ,  *((int **)__Pb__i__Pe___Type_Var_ovl_5.__Pb__i__Pe___Type::addr) = __Pb__i__Pe___Type_Var_ovl_5.__Pb__i__Pe___Type::ptr) , __Pb__i__Pe___Type_Var_ovl_5);
     *__Pb__i__Pe___Ret_Deref___Pb__i__Pe___Type_Arg(__Pb__i__Pe___Type_Ret_create_struct_from_addr_UL_Arg(((unsigned long long )(&array_ptr[counter])))) = counter;
  }
  printf("Printing array\n");
  printf("before loop7\n");
  for (counter = 0U; counter < 100U; counter++) {
    printf("%u\n", *__Pb__i__Pe___Ret_Deref___Pb__i__Pe___Type_Arg(__Pb__i__Pe___Type_Ret_create_struct_from_addr_UL_Arg(((unsigned long long )(&array_ptr[counter])))));
  }
    #endif
    #if 0
    #endif
    #endif
  return 1;
}
