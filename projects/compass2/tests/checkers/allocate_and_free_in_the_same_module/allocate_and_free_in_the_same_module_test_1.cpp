// 3 Matches
#include <stdlib.h>

struct pointer_container
{
  int *contained_ptr;
  int i;
  float f;
};

struct pointer_container2
{
  int *contained_ptr;
};

int *ret_int(int *ret_int_var)
{
  return ret_int_var;
}

int *gen_int()
{
  return (int *)malloc(sizeof(int));
}

void free_int(int *free_int_var)
{
  free(free_int_var);
}

int main()
{
  int *ptr_chain_1 = (int*)malloc(sizeof(int)); // should be free
  int *ptr_chain_2 = ptr_chain_1;
  int *ptr_chain_3 = ptr_chain_2;
  free(ptr_chain_3);

  int *func_gen_int = gen_int(); // should not be free

  int *func_free_int_chain_1 = (int*)malloc(sizeof(int)); // should be free
  int *func_free_int_chain_2 = ret_int(func_free_int_chain_1);
  free_int(func_free_int_chain_2);

  int *func_malloc = gen_int();
  free(func_malloc);

  pointer_container ctr;
  ctr.contained_ptr = gen_int(); // should be free
  pointer_container ctr2;
  ctr2.contained_ptr = ctr.contained_ptr;
  free_int(ctr2.contained_ptr);

  int *ptr = (int*)malloc(sizeof(int)); // should not be free

  pointer_container2 ctr3;
  ctr3.contained_ptr = (int*)malloc(sizeof(int)); // should not be free
  int *ptr2 = ctr3.contained_ptr;
}
