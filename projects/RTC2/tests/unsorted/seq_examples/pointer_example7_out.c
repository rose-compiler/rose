#include "foo2_c.h" 
bool Equality_IDK_691_Cast(struct VoidStruct ,unsigned int *);
bool NotEqual_IDK_691_Cast(struct VoidStruct ,unsigned int *);
struct VoidStruct Increment(struct VoidStruct ,unsigned int );
bool LessThan_IDK_691_IDK_385(struct VoidStruct ,struct VoidStruct );
struct VoidStruct Add(struct VoidStruct ,int ,unsigned int );
struct VoidStruct malloc_overload(int );
int *Deref(struct VoidStruct );
struct VoidStruct Assign(struct VoidStruct ,unsigned int *);
void create_entry(unsigned int **,unsigned long long );

struct VoidStruct 
{
  void *ptr;
  unsigned long long addr;
}
;
struct VoidStruct fn2(struct VoidStruct ,struct VoidStruct ,struct VoidStruct ,struct VoidStruct );

struct VoidStruct fn1(struct VoidStruct input_str)
{
  unsigned int *input_recr;
  (create_entry(&input_recr,input_str.addr) , ((input_str.ptr = (reinterpret_cast < void * >  (input_recr)) , input_str.addr = (reinterpret_cast < unsigned long long  >  ((&input_recr))))));
  unsigned int *nullptr;
  struct VoidStruct nullptr_str;
  ((nullptr_str.ptr = (reinterpret_cast < void * >  (nullptr)) , nullptr_str.addr = (reinterpret_cast < unsigned long long  >  ((&nullptr)))) , ((Assign(nullptr_str,fn2(input_str,((float *)input_str),((double *)input_str),((char *)input_str))) , nullptr = ((unsigned int *)nullptr_str.ptr))));
  printf("input: %u\n", *Deref(input_str));
  return (unsigned int *)(malloc_overload((100 * (sizeof(unsigned int )))));
}

struct VoidStruct fn2(struct VoidStruct input_str,struct VoidStruct input2_str,struct VoidStruct input3_str,struct VoidStruct input4_str)
{
  char *input4_recr;
  (create_entry(&input4_recr,input4_str.addr) , ((input4_str.ptr = (reinterpret_cast < void * >  (input4_recr)) , input4_str.addr = (reinterpret_cast < unsigned long long  >  ((&input4_recr))))));
  double *input3_recr;
  (create_entry(&input3_recr,input3_str.addr) , ((input3_str.ptr = (reinterpret_cast < void * >  (input3_recr)) , input3_str.addr = (reinterpret_cast < unsigned long long  >  ((&input3_recr))))));
  float *input2_recr;
  (create_entry(&input2_recr,input2_str.addr) , ((input2_str.ptr = (reinterpret_cast < void * >  (input2_recr)) , input2_str.addr = (reinterpret_cast < unsigned long long  >  ((&input2_recr))))));
  unsigned int *input_recr;
  (create_entry(&input_recr,input_str.addr) , ((input_str.ptr = (reinterpret_cast < void * >  (input_recr)) , input_str.addr = (reinterpret_cast < unsigned long long  >  ((&input_recr))))));
  return 0L;
}

int main()
{
  StartClock();
  unsigned int *ptr;
  struct VoidStruct ptr_str;
  ((ptr_str.VoidStruct::ptr = (reinterpret_cast < void * >  (ptr)) , ptr_str.addr = (reinterpret_cast < unsigned long long  >  ((&ptr)))) , ((Assign(ptr_str,((unsigned int *)(malloc_overload((100 * (sizeof(int ))))))) , ptr = ((unsigned int *)ptr_str.VoidStruct::ptr))));
  unsigned int *ptr2;
  struct VoidStruct ptr2_str;
  ((ptr2_str.VoidStruct::ptr = (reinterpret_cast < void * >  (ptr2)) , ptr2_str.addr = (reinterpret_cast < unsigned long long  >  ((&ptr2)))) , ((Assign(ptr2_str,((unsigned int *)(malloc_overload((10 * (sizeof(int ))))))) , ptr2 = ((unsigned int *)ptr2_str.VoidStruct::ptr))));
  unsigned int *start_ptr;
  struct VoidStruct start_ptr_str;
  ((start_ptr_str.VoidStruct::ptr = (reinterpret_cast < void * >  (start_ptr)) , start_ptr_str.addr = (reinterpret_cast < unsigned long long  >  ((&start_ptr)))) , ((Assign(start_ptr_str,ptr_str) , start_ptr = ((unsigned int *)start_ptr_str.VoidStruct::ptr))));
  unsigned int *start_ptr2;
  struct VoidStruct start_ptr2_str;
  ((start_ptr2_str.VoidStruct::ptr = (reinterpret_cast < void * >  (start_ptr2)) , start_ptr2_str.addr = (reinterpret_cast < unsigned long long  >  ((&start_ptr2)))) , ((Assign(start_ptr2_str,ptr2_str) , start_ptr2 = ((unsigned int *)start_ptr2_str.VoidStruct::ptr))));
  unsigned int *start_ptr3;
  struct VoidStruct start_ptr3_str;
  ((start_ptr3_str.VoidStruct::ptr = (reinterpret_cast < void * >  (start_ptr3)) , start_ptr3_str.addr = (reinterpret_cast < unsigned long long  >  ((&start_ptr3)))) , ((Assign(start_ptr3_str,((unsigned int *)(malloc_overload((100 * (sizeof(unsigned int ))))))) , start_ptr3 = ((unsigned int *)start_ptr3_str.VoidStruct::ptr))));
  unsigned int *start_ptr4;
  struct VoidStruct start_ptr4_str;
  ((start_ptr4_str.VoidStruct::ptr = (reinterpret_cast < void * >  (start_ptr4)) , start_ptr4_str.addr = (reinterpret_cast < unsigned long long  >  ((&start_ptr4)))) , ((Assign(start_ptr4_str,start_ptr2_str) , start_ptr4 = ((unsigned int *)start_ptr4_str.VoidStruct::ptr))));
    #if 1
   *Deref(start_ptr_str) = 1;
   *Deref(start_ptr2_str) = 1;
   *Deref(ptr_str) = 3;
   *Deref(ptr2_str) = 9;
    #endif
  for (unsigned int *new_ptr, struct VoidStruct new_ptr_str, ((new_ptr_str.VoidStruct::ptr = (reinterpret_cast < void * >  (new_ptr)) , new_ptr_str.addr = (reinterpret_cast < unsigned long long  >  ((&new_ptr)))) , ((Assign(new_ptr_str,start_ptr_str) , new_ptr = ((unsigned int *)new_ptr_str.VoidStruct::ptr)))); LessThan_IDK_691_IDK_385(new_ptr_str,Add(start_ptr_str,100,sizeof(int ))); ((Increment(new_ptr_str,sizeof(int )) , new_ptr = ((unsigned int *)new_ptr_str.VoidStruct::ptr)) , new_ptr_str)) {
     *Deref(new_ptr_str) = 5;
    printf("%u\n", *Deref(new_ptr_str));
  }
  unsigned int *whileptr;
  struct VoidStruct whileptr_str;
  ((whileptr_str.VoidStruct::ptr = (reinterpret_cast < void * >  (whileptr)) , whileptr_str.addr = (reinterpret_cast < unsigned long long  >  ((&whileptr)))) , ((Assign(whileptr_str,0L) , whileptr = ((unsigned int *)whileptr_str.VoidStruct::ptr))));
  do {
    unsigned int *doptr;
    struct VoidStruct doptr_str;
    ((doptr_str.VoidStruct::ptr = (reinterpret_cast < void * >  (doptr)) , doptr_str.addr = (reinterpret_cast < unsigned long long  >  ((&doptr)))) , ((Assign(doptr_str,start_ptr_str) , doptr = ((unsigned int *)doptr_str.VoidStruct::ptr))));
    for (unsigned int *forptr2, struct VoidStruct forptr2_str, ((forptr2_str.VoidStruct::ptr = (reinterpret_cast < void * >  (forptr2)) , forptr2_str.addr = (reinterpret_cast < unsigned long long  >  ((&forptr2)))) , ((Assign(forptr2_str,((unsigned int *)(malloc_overload((100 * (sizeof(unsigned int ))))))) , forptr2 = ((unsigned int *)forptr2_str.VoidStruct::ptr)))), *doptr2, struct VoidStruct doptr2_str, ((doptr2_str.VoidStruct::ptr = (reinterpret_cast < void * >  (doptr2)) , doptr2_str.addr = (reinterpret_cast < unsigned long long  >  ((&doptr2)))) , ((Assign(doptr2_str,forptr2_str) , doptr2 = ((unsigned int *)doptr2_str.VoidStruct::ptr)))); LessThan_IDK_691_IDK_385(doptr2_str,Add(forptr2_str,100,sizeof(int ))); ((Increment(doptr2_str,sizeof(int )) , doptr2 = ((unsigned int *)doptr2_str.VoidStruct::ptr)) , doptr2_str)) {
    }
  }while (NotEqual_IDK_691_Cast(whileptr_str,0L));
  unsigned int *tempptr;
  struct VoidStruct tempptr_str;
  ((tempptr_str.VoidStruct::ptr = (reinterpret_cast < void * >  (tempptr)) , tempptr_str.addr = (reinterpret_cast < unsigned long long  >  ((&tempptr)))) , ((Assign(tempptr_str,start_ptr_str) , tempptr = ((unsigned int *)tempptr_str.VoidStruct::ptr))));
  if (Equality_IDK_691_Cast(whileptr_str,0L)) {
    start_ptr_str += (100 - 1);
     *Deref(start_ptr_str) = 10;
  }
  ((Assign(start_ptr_str,tempptr_str) , start_ptr = ((unsigned int *)start_ptr_str.VoidStruct::ptr)) , start_ptr_str);
  printf("Final print\n");
  for (unsigned int *new_ptr, struct VoidStruct new_ptr_str, ((new_ptr_str.VoidStruct::ptr = (reinterpret_cast < void * >  (new_ptr)) , new_ptr_str.addr = (reinterpret_cast < unsigned long long  >  ((&new_ptr)))) , ((Assign(new_ptr_str,start_ptr_str) , new_ptr = ((unsigned int *)new_ptr_str.VoidStruct::ptr)))); LessThan_IDK_691_IDK_385(new_ptr_str,Add(start_ptr_str,100,sizeof(int ))); ((Increment(new_ptr_str,sizeof(int )) , new_ptr = ((unsigned int *)new_ptr_str.VoidStruct::ptr)) , new_ptr_str)) {
    printf("%u\n", *Deref(new_ptr_str));
  }
  printf("Final print -end\n");
// CIRM Review Code
  unsigned int *p;
  struct VoidStruct p_str;
  ((p_str.VoidStruct::ptr = (reinterpret_cast < void * >  (p)) , p_str.addr = (reinterpret_cast < unsigned long long  >  ((&p)))) , ((Assign(p_str,((unsigned int *)(malloc_overload((100 * (sizeof(unsigned int ))))))) , p = ((unsigned int *)p_str.VoidStruct::ptr))));
  unsigned int *q;
  struct VoidStruct q_str;
  (q_str.VoidStruct::ptr = (reinterpret_cast < void * >  (q)) , q_str.addr = (reinterpret_cast < unsigned long long  >  ((&q))));
  unsigned int y = ((((((Assign(q_str,p_str) , q = ((unsigned int *)q_str.VoidStruct::ptr)) , q_str) ,  *Deref(q_str) = 5)) ,  *Deref(q_str)));
// ------
// Passing pointers to function
  unsigned int *fnptr;
  struct VoidStruct fnptr_str;
  ((fnptr_str.VoidStruct::ptr = (reinterpret_cast < void * >  (fnptr)) , fnptr_str.addr = (reinterpret_cast < unsigned long long  >  ((&fnptr)))) , ((Assign(fnptr_str,fn1(start_ptr_str)) , fnptr = ((unsigned int *)fnptr_str.VoidStruct::ptr))));
  unsigned int *fnptr2;
  struct VoidStruct fnptr2_str;
  (fnptr2_str.VoidStruct::ptr = (reinterpret_cast < void * >  (fnptr2)) , fnptr2_str.addr = (reinterpret_cast < unsigned long long  >  ((&fnptr2))));
  ((Assign(fnptr2_str,fn1(fnptr_str)) , fnptr2 = ((unsigned int *)fnptr2_str.VoidStruct::ptr)) , fnptr2_str);
// ------
// Complex expressions
  unsigned int *complexptr;
  struct VoidStruct complexptr_str;
  ((complexptr_str.VoidStruct::ptr = (reinterpret_cast < void * >  (complexptr)) , complexptr_str.addr = (reinterpret_cast < unsigned long long  >  ((&complexptr)))) , ((Assign(complexptr_str,Add(start_ptr_str,1,sizeof(int ))) , complexptr = ((unsigned int *)complexptr_str.VoidStruct::ptr))));
  unsigned int complexexp = (( *Deref(ptr_str) +  *Deref(fn1((((Increment(start_ptr_str,sizeof(int )) , start_ptr = ((unsigned int *)start_ptr_str.VoidStruct::ptr)) , start_ptr_str))))) -  *Deref(p_str));
// Add more
  EndClock();
  return 1;
}
