#include "foo2_c.h" 
bool Equality_IDK_691_IDK_385(struct VoidStruct ,struct VoidStruct );
bool NotEqual_IDK_691_IDK_385(struct VoidStruct ,struct VoidStruct );
struct VoidStruct Increment(struct VoidStruct ,unsigned int );
bool LessThan_IDK_691_IDK_385(struct VoidStruct ,struct VoidStruct );
struct VoidStruct Add(struct VoidStruct ,int ,unsigned int );
struct VoidStruct Cast(struct VoidStruct );
struct VoidStruct malloc_overload(unsigned int );
struct VoidStruct Assign(struct VoidStruct ,int *);

struct VoidStruct 
{
  void *ptr;
  unsigned long long addr;
}
;
struct VoidStruct fn2(struct VoidStruct ,struct VoidStruct ,struct VoidStruct ,struct VoidStruct );

struct VoidStruct fn1(struct VoidStruct input_str)
{
  int *input_recr;
  input_recr = ((int *)input_str.ptr);
  int *nullptr;
  struct VoidStruct nullptr_str;
  ((nullptr_str.ptr = (reinterpret_cast < void * >  (nullptr)) , nullptr_str.addr = (reinterpret_cast < unsigned long long  >  ((&nullptr)))) , ((Assign(nullptr_str,fn2(input_str,input_str,input_str,input_str)) , nullptr = ((int *)nullptr_str.ptr))));
  printf("input: %u\n", *input_str);
  return Cast(malloc_overload((100 * (sizeof(int )))));
}

struct VoidStruct fn2(struct VoidStruct input_str,struct VoidStruct input2_str,struct VoidStruct input3_str,struct VoidStruct input4_str)
{
  int *input4_recr;
  input4_recr = ((int *)input4_str.ptr);
  int *input3_recr;
  input3_recr = ((int *)input3_str.ptr);
  int *input2_recr;
  input2_recr = ((int *)input2_str.ptr);
  int *input_recr;
  input_recr = ((int *)input_str.ptr);
  return Cast(0);
}

int main()
{
  StartClock();
  int *ptr;
  struct VoidStruct ptr_str;
  ((ptr_str.VoidStruct::ptr = (reinterpret_cast < void * >  (ptr)) , ptr_str.addr = (reinterpret_cast < unsigned long long  >  ((&ptr)))) , ((Assign(ptr_str,Cast(malloc_overload((100 * (sizeof(int )))))) , ptr = ((int *)ptr_str.VoidStruct::ptr))));
  int *ptr2;
  struct VoidStruct ptr2_str;
  ((ptr2_str.VoidStruct::ptr = (reinterpret_cast < void * >  (ptr2)) , ptr2_str.addr = (reinterpret_cast < unsigned long long  >  ((&ptr2)))) , ((Assign(ptr2_str,Cast(malloc_overload((10 * (sizeof(int )))))) , ptr2 = ((int *)ptr2_str.VoidStruct::ptr))));
  int *start_ptr;
  struct VoidStruct start_ptr_str;
  ((start_ptr_str.VoidStruct::ptr = (reinterpret_cast < void * >  (start_ptr)) , start_ptr_str.addr = (reinterpret_cast < unsigned long long  >  ((&start_ptr)))) , ((Assign(start_ptr_str,ptr_str) , start_ptr = ((int *)start_ptr_str.VoidStruct::ptr))));
  int *start_ptr2;
  struct VoidStruct start_ptr2_str;
  ((start_ptr2_str.VoidStruct::ptr = (reinterpret_cast < void * >  (start_ptr2)) , start_ptr2_str.addr = (reinterpret_cast < unsigned long long  >  ((&start_ptr2)))) , ((Assign(start_ptr2_str,ptr2_str) , start_ptr2 = ((int *)start_ptr2_str.VoidStruct::ptr))));
  int *start_ptr3;
  struct VoidStruct start_ptr3_str;
  ((start_ptr3_str.VoidStruct::ptr = (reinterpret_cast < void * >  (start_ptr3)) , start_ptr3_str.addr = (reinterpret_cast < unsigned long long  >  ((&start_ptr3)))) , ((Assign(start_ptr3_str,Cast(malloc_overload((100 * (sizeof(int )))))) , start_ptr3 = ((int *)start_ptr3_str.VoidStruct::ptr))));
  int *start_ptr4;
  struct VoidStruct start_ptr4_str;
  ((start_ptr4_str.VoidStruct::ptr = (reinterpret_cast < void * >  (start_ptr4)) , start_ptr4_str.addr = (reinterpret_cast < unsigned long long  >  ((&start_ptr4)))) , ((Assign(start_ptr4_str,start_ptr2_str) , start_ptr4 = ((int *)start_ptr4_str.VoidStruct::ptr))));
    #if 1
   *start_ptr_str = 1;
   *start_ptr2_str = 1;
   *ptr_str = 3;
   *ptr2_str = 9;
    #endif
  for (int *new_ptr, struct VoidStruct new_ptr_str, ((new_ptr_str.VoidStruct::ptr = (reinterpret_cast < void * >  (new_ptr)) , new_ptr_str.addr = (reinterpret_cast < unsigned long long  >  ((&new_ptr)))) , ((Assign(new_ptr_str,start_ptr_str) , new_ptr = ((int *)new_ptr_str.VoidStruct::ptr)))); LessThan_IDK_691_IDK_385(new_ptr_str,Add(start_ptr_str,100,sizeof(char ))); Increment(new_ptr_str,sizeof(char ))) {
     *new_ptr_str = 5;
    printf("%u\n", *new_ptr_str);
  }
  int *whileptr;
  struct VoidStruct whileptr_str;
  ((whileptr_str.VoidStruct::ptr = (reinterpret_cast < void * >  (whileptr)) , whileptr_str.addr = (reinterpret_cast < unsigned long long  >  ((&whileptr)))) , ((Assign(whileptr_str,Cast(0)) , whileptr = ((int *)whileptr_str.VoidStruct::ptr))));
  do {
    int *doptr;
    struct VoidStruct doptr_str;
    ((doptr_str.VoidStruct::ptr = (reinterpret_cast < void * >  (doptr)) , doptr_str.addr = (reinterpret_cast < unsigned long long  >  ((&doptr)))) , ((Assign(doptr_str,start_ptr_str) , doptr = ((int *)doptr_str.VoidStruct::ptr))));
    for (int *forptr2, struct VoidStruct forptr2_str, ((forptr2_str.VoidStruct::ptr = (reinterpret_cast < void * >  (forptr2)) , forptr2_str.addr = (reinterpret_cast < unsigned long long  >  ((&forptr2)))) , ((Assign(forptr2_str,Cast(malloc_overload((100 * (sizeof(int )))))) , forptr2 = ((int *)forptr2_str.VoidStruct::ptr)))), *doptr2, struct VoidStruct doptr2_str, ((doptr2_str.VoidStruct::ptr = (reinterpret_cast < void * >  (doptr2)) , doptr2_str.addr = (reinterpret_cast < unsigned long long  >  ((&doptr2)))) , ((Assign(doptr2_str,forptr2_str) , doptr2 = ((int *)doptr2_str.VoidStruct::ptr)))); LessThan_IDK_691_IDK_385(doptr2_str,Add(forptr2_str,100,sizeof(char ))); Increment(doptr2_str,sizeof(char ))) {
    }
  }while (NotEqual_IDK_691_IDK_385(whileptr_str,Cast(0)));
  int *tempptr;
  struct VoidStruct tempptr_str;
  ((tempptr_str.VoidStruct::ptr = (reinterpret_cast < void * >  (tempptr)) , tempptr_str.addr = (reinterpret_cast < unsigned long long  >  ((&tempptr)))) , ((Assign(tempptr_str,start_ptr_str) , tempptr = ((int *)tempptr_str.VoidStruct::ptr))));
  if (Equality_IDK_691_IDK_385(whileptr_str,Cast(0))) {
    start_ptr_str += (100 - 1);
     *start_ptr_str = 10;
  }
  Assign(start_ptr_str,tempptr_str);
  printf("Final print\n");
  for (int *new_ptr, struct VoidStruct new_ptr_str, ((new_ptr_str.VoidStruct::ptr = (reinterpret_cast < void * >  (new_ptr)) , new_ptr_str.addr = (reinterpret_cast < unsigned long long  >  ((&new_ptr)))) , ((Assign(new_ptr_str,start_ptr_str) , new_ptr = ((int *)new_ptr_str.VoidStruct::ptr)))); LessThan_IDK_691_IDK_385(new_ptr_str,Add(start_ptr_str,100,sizeof(char ))); Increment(new_ptr_str,sizeof(char ))) {
    printf("%u\n", *new_ptr_str);
  }
  printf("Final print -end\n");
// CIRM Review Code
  int *p;
  struct VoidStruct p_str;
  ((p_str.VoidStruct::ptr = (reinterpret_cast < void * >  (p)) , p_str.addr = (reinterpret_cast < unsigned long long  >  ((&p)))) , ((Assign(p_str,Cast(malloc_overload((100 * (sizeof(int )))))) , p = ((int *)p_str.VoidStruct::ptr))));
  int *q;
  struct VoidStruct q_str;
  (q_str.VoidStruct::ptr = (reinterpret_cast < void * >  (q)) , q_str.addr = (reinterpret_cast < unsigned long long  >  ((&q))));
  int y = ((((Assign(q_str,p_str) ,  *q_str = 5)) ,  *q_str));
// ------
// Passing pointers to function
  int *fnptr;
  struct VoidStruct fnptr_str;
  ((fnptr_str.VoidStruct::ptr = (reinterpret_cast < void * >  (fnptr)) , fnptr_str.addr = (reinterpret_cast < unsigned long long  >  ((&fnptr)))) , ((Assign(fnptr_str,fn1(start_ptr_str)) , fnptr = ((int *)fnptr_str.VoidStruct::ptr))));
  int *fnptr2;
  struct VoidStruct fnptr2_str;
  (fnptr2_str.VoidStruct::ptr = (reinterpret_cast < void * >  (fnptr2)) , fnptr2_str.addr = (reinterpret_cast < unsigned long long  >  ((&fnptr2))));
  Assign(fnptr2_str,fn1(fnptr_str));
// ------
// Complex expressions
  int *complexptr;
  struct VoidStruct complexptr_str;
  ((complexptr_str.VoidStruct::ptr = (reinterpret_cast < void * >  (complexptr)) , complexptr_str.addr = (reinterpret_cast < unsigned long long  >  ((&complexptr)))) , ((Assign(complexptr_str,Add(start_ptr_str,1,sizeof(char ))) , complexptr = ((int *)complexptr_str.VoidStruct::ptr))));
  int complexexp = (( *ptr_str +  *fn1(Increment(start_ptr_str,sizeof(char )))) -  *p_str);
// Add more
  EndClock();
  return 1;
}
