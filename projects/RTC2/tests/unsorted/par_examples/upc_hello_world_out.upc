#include <upc.h> /* Required for UPC extensions */
#include <stdio.h>
static char* global_var0_5_9 = "Hello from %d of %d\n";

struct __Pb__Cc__Pe___Type 
{
  const char *ptr;
  unsigned long long addr;
}
;
static struct __Pb__Cc__Pe___Type __Pb__Cc__Pe___Type_Ret_Cast___Pb__Cc__Pe___Arg_UL_Arg_Ul_Arg_UL_Arg(const char *,unsigned long long ,unsigned long ,unsigned long long );
void v_Ret_create_entry_UL_Arg_UL_Arg_Ul_Arg_UL_Arg(unsigned long long ,unsigned long long ,unsigned long ,unsigned long long );
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
void v_Ret_execAtFirst();
void v_Ret_execAtLast();

int main()
{
  v_Ret_execAtFirst();
  printf(__Pb__Cc__Pe___Ret_return_pointer___Pb__Cc__Pe___Type_Arg(__Pb__Cc__Pe___Type_Ret_Cast___Pb__Cc__Pe___Arg_UL_Arg_Ul_Arg_UL_Arg(((const char *)global_var0_5_9),((unsigned long long )(&global_var0_5_9)),((unsigned long )(sizeof(global_var0_5_9))),((unsigned long long )50))),(MYTHREAD ),(THREADS ));
  v_Ret_execAtLast();
  return 1;
}
