#include "rose_config.h"
#include "libxomp.h"

#ifdef USE_ROSE_GOMP_OPENMP_LIBRARY 

// GOMP header
#include "libgomp_g.h"
// GOMP has higher precedence if both runtime libraries' path are specified
#undef USE_ROSE_OMNI_OPENMP_SUPPORT

#else
// Omni header
#include "libompc.h"
#endif

// avoid include omp.h 
extern int omp_get_thread_num(void);
extern int omp_get_num_threads(void);

#include <stdlib.h> // for getenv(), malloc(), etc
#include <stdio.h> // for getenv()
#include <assert.h>
#include <stdarg.h>
#include <string.h> // for memcpy()

#if 0
enum omp_rtl_enum {
  e_undefined,
  e_gomp,
  e_omni,
  e_last_rtl
};

//extern omp_rtl_enum rtl_type;
enum omp_rtl_enum rtl_type;

#if 0
#ifdef __cplusplus
extern "C" {
/* There appears to be no closing brace for the extern "C". I'm not sure where it was intended to go, so I'm adding it here so
 * that curly braces balance out. [RPM 2010-11-11] */
}
#endif
#endif 

//! return the current runtime library (RTL) type 
// Read environment variable once
//  Not in use since the undefined reference problem if either of the libraries are not available
static enum omp_rtl_enum get_rtl_type()
{
  static enum omp_rtl_enum t = e_undefined;
  if (t == e_undefined)
  { // possible value: GOMP, OMNI
    char* e_value = getenv("ROSE_OMP_RTL_TYPE");
    if ((e_value[0] == 'G')&& (e_value[1] == 'O') && (e_value[2] == 'M') && (e_value[3] == 'P'))
      t = e_gomp;
    else if ((e_value[0] == 'O')&& (e_value[1] == 'M') && (e_value[2] == 'N') && (e_value[3] == 'I'))
      t = e_omni;
    else 
    {
      printf ("unset or unrecognized OpenMP runtime type:%s\n", e_value);
      assert (0);
    }
  }
  return t;
}

#endif
// Nothing is needed for Fortran case
#pragma weak xomp_init_=xomp_init
void xomp_init (void)
{
}

//Runtime library initialization routine
void XOMP_init (int argc, char ** argv)
{
#ifdef USE_ROSE_GOMP_OPENMP_LIBRARY  
#else   
  _ompc_init (argc, argv);
#endif    
}

void xomp_terminate (int exitcode);
#pragma weak xomp_terminate_=xomp_terminate
void xomp_terminate (int exitcode)
{
  XOMP_terminate (exitcode);
}
// Runtime library termination routine
void XOMP_terminate (int exitcode)
{
#ifdef USE_ROSE_GOMP_OPENMP_LIBRARY  
#else   
  _ompc_terminate (exitcode);
#endif    
}

// array of pointers of void
// global is feasible since we don't support nested parallelism yet
void *g_parameter[MAX_OUTLINED_FUNC_PARAMETER_COUNT];
// void **g_parameter;

// run_me_## () can be called by either XOMP_parallel_start() or XOMP_task()
#if 0
static void run_me (void*);
// a helper function to deal with one-to-multiple parameter mapping 
// from C runtime (one void * data) to Fortran outlined function (multiple parameters)
void run_me(void* data)
{
  //unwrap both function pointer and parameters to be passed
  //  void *l_parameter[2]; // must use a local copy here !!
  //  void (*func) (void *, void *);
  //func =(void (*) (void *, void *)) (((void**)data)[0]);
 
  typedef void (*FUNC_P)(void *, void *);  
  FUNC_P func;
  func =(FUNC_P) (((void**)data)[0]);
  func(((void**)data)[1], ((void**)data)[2]);
}
#endif
#include "run_me_defs.inc"

void xomp_parallel_start (void (*func) (void *), unsigned* numThread, int * argcount, ...);
//Wrapper functions to support linking with Fortran programs
//
// OFP does not support extensions like %VAL, so we use XOMP to compensate the difference
// between pass-by-reference vs. pass-by-value.
//void xomp_parallel_start (void (*func) (void *), void *data, unsigned numThread)
#pragma weak xomp_parallel_start_=xomp_parallel_start
//void xomp_parallel_start (void (*func) (void *), void *data, unsigned* numThread)
//
// We expect the outlined function from a Fortran task will have multiple parameters passed by references
// This wrapper function will pack those parameters into a single one to be compatible with gomp's parallel_start function
// A glue part from Fortran's multiple parameters --> XOMP's single parameter
void xomp_parallel_start (void (*func) (void *), unsigned* numThread, int * argcount, ...)
{
  int x;
  va_list v1;
  int p_offset=1; // the first position is reserved to the function pointer to the outlined Fortran subroutine

  // simplest case where no shared variables to be passed.
  if (*argcount == 0)
  {

    XOMP_parallel_start (func, 0, *numThread);
    return;
  }
  
  // Handle one or more shared variables
   
  //TODO nested parallelism ??
//  int parameter_count = *argcount +1; 
//  g_parameter = (void**) malloc(sizeof(void*) * parameter_count);
//  Unreliable malloc?  seg fault. Use predefined size instead
//  if (g_parameter == (void**)0)
//  {
//    printf("xomp.c xomp_parallel_start(), malloc failed for %d parameters.\n", parameter_count);
//    exit (3);
//  }

  printf("Debug: xomp_parallel_start(), g_parameter at address %p\n", g_parameter);
  g_parameter[0]= func;
  va_start (v1, (*argcount));
  for (x=0; x<(*argcount); x++)
  {
    g_parameter[p_offset+x]= va_arg(v1, void*);
  }

  switch (*argcount)
  {
//   XOMP_parallel_start (run_me_?, (void*) g_parameter, *numThread);
   #include "run_me_callers.inc"
   default:
    printf("Error. Unhandled number of parameters %d \n", *argcount);
    assert (0);
    break;
  }
}

void XOMP_parallel_start (void (*func) (void *), void *data, unsigned numThread)
{

#ifdef USE_ROSE_GOMP_OPENMP_LIBRARY  
  GOMP_parallel_start (func, data, numThread);
  func(data);
#else   
  _ompc_do_parallel ((void (*)(void **))func, data); 
#endif    
}

void xomp_parallel_end (void);
#pragma weak xomp_parallel_end_=xomp_parallel_end
void xomp_parallel_end (void)
{
  //TODO nested parallelism
  //free(g_parameter);
  XOMP_parallel_end ();
}
void XOMP_parallel_end (void)
{
#ifdef USE_ROSE_GOMP_OPENMP_LIBRARY  
  GOMP_parallel_end ();
#else   
#endif    
}

//----------------
#include "run_me_task_defs.inc"
// statically allocated pg_parameter
#define MAX_XOMP_TASK_NUMBER 99999 // cannot be too large, otherwise exceed static allocation limit
void *task_parameter[MAX_XOMP_TASK_NUMBER][MAX_OUTLINED_FUNC_PARAMETER_COUNT];
void xomp_task(void (*func) (void *), void (*cpyfn) (void *, void *), int* arg_size, int* arg_align, 
               int* if_clause, int* untied, int * argcount, ...);
#pragma weak xomp_task_=xomp_task
// Main tasks: 
//   convert Fortran parameter types to the right C one 
//   wrap function pointer to a outlined function routine and all its parameters into one single parameter gp_parameter[]
//   Call the real xomp_task () with the bridge run_me_task_#() function
void xomp_task(void (*func) (void *), void (*cpyfn) (void *, void *), int* arg_size, int* arg_align, 
               int* if_clause, int* untied, int * argcount, ...)
{
  int x = 0;
  int p_offset =0;

  // Liao 1/25/2010
  // I don't know yet a way to pass NULL function pointer from Fortran to C.
  // The 0 integer generated by the lowering process will be passed by its address, which is not NULL. 
  // So I set it to 0 here since this is the only reasonable value for the translation we use.
  // TODO find a better way to handle cpyfn pointer when it is NULL, memcpy?
  cpyfn = 0; 
  long largcount = (long) (*argcount)/3; // divide by 3 to get real number since we pass each parameter with a triplet form(by-value, value-size, address)
  assert ((largcount * 3) == *argcount);
  long larg_size = (long) ( *arg_size);
  long larg_align= (long) (*arg_align);
  bool bif_clause = (bool) (*if_clause);
  unsigned uuntied = (unsigned) (*untied);

  // simplest case where no shared variables to be passed.
  if (largcount == 0)
  {
    // void XOMP_task (void (*fn) (void *), void *data, void (*cpyfn) (void *, void *), long arg_size, long arg_align, bool if_clause, unsigned untied)
    //    printf("xomp.c xomp_task(), 0 variables to be passed. func = %p data = %p, cpyfn = %p\n", func, NULL, cpyfn );
    XOMP_task(func, NULL, cpyfn, larg_size, larg_align, bif_clause, uuntied);
    return;
  }

  // Common case where there are one or more private/shared variables to be passed
  // Adjust parameter count and size: a function pointer + its original parameters
  int parameter_count = largcount +1;  // todo REMOVE this variable

  // TODO: iteration here to calculate real size, or let compiler calculate it and pass it here
  // assume 512 bytes for now
  larg_size = 512; // 40+ 4+ 8 = 52, larg_size + sizeof(void*);  // variable size + function_pointer size
  // We need to use dedicated private g_parameter for each task. 
  // Single the parent function of tasks may die out before children tasks begin execution: untied case
  char * pg_parameter = (char*) malloc(larg_size); // allocate 8+4 =12 bytes for pg_parameter, this is the data to be coped into each task!
  //char * pg_parameter = (char*) malloc(larg_size); // allocate 8+4 =12 bytes for pg_parameter, this is the data to be coped into each task!
  // save mixed addresses and values as needed. 
  if (pg_parameter == (void*)0)
  {
    printf("xomp.c xomp_task(), malloc failed for %d parameters.\n", parameter_count);
    exit (3);
  }
  else
  {
    //    printf("Debug: xomp_task(), allocated pg_parameter at address %p , void* address %p, for func %p \n", pg_parameter, (void*)pg_parameter, func);
  }

  // first parameter ----------------
  // Fill in the Fortran subroutine pointer, which will be size(void*)
  memcpy(pg_parameter, &func, sizeof(void*)); // copy function pointer to pg_parameter
  p_offset+= sizeof(void*); // the two position (8 bytes) is reserved to the function pointer to the outlined Fortran subroutine

  // Grab data parameters for the Fortran subroutine one by one
  va_list v1;
  va_start (v1, (*argcount));

  // 2nd parameter and after ----------------
  // Fill in the actual data parameters for the function pointer
  for (x=0; x<largcount; x++)
  { 
     // grab 1st of the triplet: pass-by-value or not
    int by_value = *(int*)(va_arg(v1, void*));

     // grab 2nd of the triplet: size of the value/address
    int v_size =  *(int*)(va_arg(v1, void*));
    if (!by_value)
    { // verify size of address
      assert (v_size == sizeof(void*));
    }

    // 3rd of the triplet: address (of the value) : store value or address as required 
    if (by_value)
    {
      char * i = (char*)(va_arg(v1, void*)); // every thing is a pointer from Fortran to xomp
      memcpy (&(pg_parameter[p_offset]),i, v_size);
    }  
    else
    {
      // address
      void * i2 = (void*) (va_arg(v1, void*)); 
      memcpy (&(pg_parameter[p_offset]),&i2, v_size);
    //  printf ("Debug, fortran xomp, array address is %p\n", i2);
    }
    p_offset+=v_size; // shift offset
  }

  // 2nd parameter ----------------
#if 0   
  // address
   void * i2 = (void*) (va_arg(v1, void*)); 
   memcpy (&(pg_parameter[p_offset]),&i2, sizeof(void*));
   printf ("Debug, fortran xomp, array address is %p\n", i2);

// #else
   // value, get its address
   char * i2 = (char*)(va_arg(v1, void*)); // every thing is a pointer from Fortran to xomp
   //gstruct_40 i2 = *(gstruct_40*)(va_arg(v1, void*)); // every thing is a pointer from Fortran to xomp
   //copy all bytes of the value
   memcpy (&(pg_parameter[p_offset]),i2, 40);
   //memcpy (&(pg_parameter[p_offset]),&i2, 40);
   p_offset+=40;
   // debug the array copied
   int* item = (int*) (i2);
   int ii;
   for (ii = 0; ii<10; ii++)
     printf ("item [%d]=%d\n", ii, item[ii]);
#endif

//  printf ("Debug xomp()  largcount = %d\n", largcount);
  switch (largcount)
  {
    //   XOMP_task(run_me_task_?, (void*) pg_parameter, cpyfn, *arg_size, *arg_align, *if_clause, *untied);
#include "run_me_callers2.inc"
    default:
      printf("Error. xomp_task(): unhandled number of parameters %d \n", *argcount);
      assert (0);
      break;
  }
}

void XOMP_task (void (*fn) (void *), void *data, void (*cpyfn) (void *, void *),
                       long arg_size, long arg_align, bool if_clause, unsigned untied)
{

#ifdef USE_ROSE_GOMP_OPENMP_LIBRARY  
//// only gcc 4.4.x has task support
// It is fine to have older gcc to compile this, just remember to link with gcc 4.4 and beyond
//#if __GNUC__ > 4 || \  //
//  (__GNUC__ == 4 && (__GNUC_MINOR__ > 4 || \  //
//                     (__GNUC_MINOR__ == 4 && \  //
//                      __GNUC_PATCHLEVEL__ >= 0)))

 GOMP_task (fn, data, cpyfn, arg_size, arg_align, if_clause, untied);
//#endif

#else
#endif 
}
void XOMP_taskwait (void)
{
#ifdef USE_ROSE_GOMP_OPENMP_LIBRARY  
//#if __GNUC__ > 4 || \           //
//  (__GNUC__ == 4 && (__GNUC_MINOR__ > 4 || \  //
//                     (__GNUC_MINOR__ == 4 && \  //
//                      __GNUC_PATCHLEVEL__ >= 0)))
//
  GOMP_taskwait();
//#endif

#else
#endif 
}
//===================================== loop scheduling ==============================================
// 2^31 -1 for 32-bit integer
//#define MAX_SIGNED_INT ((int)(1<< (sizeof(int)*8-1)) -1)
#define MAX_SIGNED_INT 2147483647l
// -2^31
//#define MIN_SIGNED_INT (0-(int)(1<< (sizeof(int)*8-1)))
#define MIN_SIGNED_INT -2147483648l

#define CHECK_SIGNED_INT_RANGE(x) assert((x>=MIN_SIGNED_INT) && (x<=MAX_SIGNED_INT))

// -------default scheduling ----------------------
//Accommodate Fortran issues: underscore, small case, pass-by-reference
void xomp_loop_default (int* lower, int* upper, int* stride, int *n_lower, int * n_upper);
#pragma weak xomp_loop_default_=xomp_loop_default
void xomp_loop_default (int* lower, int* upper, int* stride, int *n_lower, int * n_upper)
{ // deal with mismatch between int and long int.
  long l_lower, l_upper;
  XOMP_loop_default (*lower, *upper, *stride, &l_lower, &l_upper);
  CHECK_SIGNED_INT_RANGE(l_lower);
  CHECK_SIGNED_INT_RANGE(l_upper);
  *n_lower = l_lower;
  *n_upper = l_upper;
}

//Default loop scheduling, worksharing without any schedule clause
// input upper bound is inclusive (loop normalized with <= or >=)
// output n_upper is also inclusive 
// stride is positive for incremental, negative for decremental iteration space
extern void XOMP_loop_default(int lower, int upper, int stride, long* n_lower, long* n_upper)
{
  int _p_lower;
  int _p_upper;
  int _p_chunk_size;
  int addOne ; // adjustment to input and output upper bounds, depending on if they are inclusive or non-inclusive

  int isDecremental= 0;
  if (lower>upper)
    isDecremental = 1;  

  // calculate loop iteration count from lower, upper and stride , 
  // adjust inclusive stride here TODO
  // no -1/+1? if upper is already an inclusive bound
  int _p_iter_count = 0;
  if (isDecremental == 1)
  {
    addOne = 1;
    //stride = 0 - stride;  // n
    if (stride >0)
    {
      printf("Error: in XOMP_loop_default() of xomp.c: stride must be negative for decremental iteration. stride = %d \n ", stride);
      assert (0);
    }
  }
  else // incremental
  {
    addOne = -1; // real bound should be non-inclusive - 1
    if (stride <0)
    {
      printf("Error: in XOMP_loop_default() of xomp.c: stride must be positive for incremental iteration. stride = %d \n ", stride);
      assert (0);
    }
  }
  // addOne is needed here only if the input upper bound is non-inclusive
  // we use loop normalization to ensure upper bounds are inclusive already.
  // So we don't need addOne here anymore
  //_p_iter_count = ( stride + addOne + upper -  lower) /  stride;
  _p_iter_count = ( stride + upper -  lower) /  stride;

  // calculate a proper chunk size
  // two cases: evenly divisible  20/5 =4
  //   not evenly divisible 20/3= 6
  // Initial candidate  

  int _p_num_threads = omp_get_num_threads();

  _p_chunk_size = _p_iter_count / _p_num_threads;
  int _p_ck_temp = _p_chunk_size * _p_num_threads != _p_iter_count;

  _p_chunk_size = _p_ck_temp + _p_chunk_size;

  // decide on the lower and upper bound for the current thread
  int _p_thread_id = omp_get_thread_num();

//  printf("inside xomp_loop_default(): _p_thread_id =%d\n", _p_thread_id);
  _p_lower =  lower + _p_chunk_size * _p_thread_id *  stride;

  //addOne is needed here if the output upper bound is inclusive
  // -1 if the output n_upper is an inclusive bound, 
  // we do use inclusive in the final normalized loop
  _p_upper = _p_lower + _p_chunk_size *  stride + addOne;

  // adjust inclusive stride here 
  // addOne is needed if the input upper bound is non-inclusive
  // no -1/+1 since upper is already an inclusive bound
  if (isDecremental == 1)
    _p_upper = (_p_upper > (upper ) ?_p_upper : (upper ));
    //_p_upper = (_p_upper > (upper + addOne) ?_p_upper : (upper + addOne));
  else
    _p_upper = (_p_upper < upper ?_p_upper : upper);
    //_p_upper = (_p_upper < (upper + addOne) ?_p_upper : (upper + addOne));

  *n_lower = _p_lower;
  *n_upper = _p_upper;
//  printf("inside xomp_loop_default(): _p_lower=%d, _p_upper=%d\n", _p_lower,_p_upper);
}


//---------------- init-----------------------------------------
//---------- static--------------
//Glue to support Fortran
void xomp_loop_static_init(int* lower, int* upper, int* stride, int* chunk_size);
#pragma weak xomp_loop_static_init_=xomp_loop_static_init
void xomp_loop_static_init(int* lower, int* upper, int* stride, int* chunk_size)
{
  XOMP_loop_static_init (*lower, *upper, *stride, *chunk_size);
}
// scheduler initialization, only meaningful used for OMNI
void XOMP_loop_static_init(int lower, int upper, int stride, int chunk_size)
{
#ifdef USE_ROSE_GOMP_OPENMP_LIBRARY  
  // empty operation for gomp
#else   
  // adjust inclusive upper bounds of XOMP to non inclusive bounds of GOMP and OMNI
  if (stride>0)
    upper ++; //+1 to be non-inclusive for an incremental iteration space
  else
    upper --;
   _ompc_static_sched_init (lower, upper, stride, chunk_size);
#endif    
}

//---------- dynamic--------------
//Glue to support Fortran
void xomp_loop_dynamic_init(int* lower, int* upper, int* stride, int* chunk_size);
#pragma weak xomp_loop_dynamic_init_=xomp_loop_dynamic_init
void xomp_loop_dynamic_init(int* lower, int* upper, int* stride, int* chunk_size)
{
  XOMP_loop_dynamic_init (*lower, *upper, *stride, *chunk_size);
}
// scheduler initialization, only meaningful used for OMNI
void XOMP_loop_dynamic_init(int lower, int upper, int stride, int chunk_size)
{
#ifdef USE_ROSE_GOMP_OPENMP_LIBRARY  
  // empty operation for gomp
#else
  // adjust inclusive upper bounds of XOMP to non inclusive bounds of GOMP and OMNI
  if (stride>0)
    upper ++; //+1 to be non-inclusive for an incremental iteration space
  else
    upper --;
   _ompc_dynamic_sched_init (lower, upper, stride, chunk_size);
#endif
}

//---------- guided--------------
//Glue to support Fortran
void xomp_loop_guided_init(int* lower, int* upper, int* stride, int* chunk_size);
#pragma weak xomp_loop_guided_init_=xomp_loop_guided_init
void xomp_loop_guided_init(int* lower, int* upper, int* stride, int* chunk_size)
{
  XOMP_loop_guided_init (*lower, *upper, *stride, *chunk_size);
}
// scheduler initialization, only meaningful used for OMNI
void XOMP_loop_guided_init(int lower, int upper, int stride, int chunk_size)
{
#ifdef USE_ROSE_GOMP_OPENMP_LIBRARY  
  // empty operation for gomp
#else
  // adjust inclusive upper bounds of XOMP to non inclusive bounds of GOMP and OMNI
  if (stride>0)
    upper ++; //+1 to be non-inclusive for an incremental iteration space
  else
    upper --;
   _ompc_guided_sched_init (lower, upper, stride, chunk_size);
#endif
}

//---------- runtime--------------
//Glue to support Fortran
void xomp_loop_runtime_init(int* lower, int* upper, int* stride);
#pragma weak xomp_loop_runtime_init_=xomp_loop_runtime_init
void xomp_loop_runtime_init(int* lower, int* upper, int* stride)
{
  XOMP_loop_runtime_init (*lower, *upper, *stride);
}
// scheduler initialization, only meaningful used for OMNI
void XOMP_loop_runtime_init(int lower, int upper, int stride)
{
#ifdef USE_ROSE_GOMP_OPENMP_LIBRARY  
  // empty operation for gomp
#else
  // adjust inclusive upper bounds of XOMP to non inclusive bounds of GOMP and OMNI
  if (stride>0)
    upper ++; //+1 to be non-inclusive for an incremental iteration space
  else
    upper --;
   _ompc_runtime_sched_init (lower, upper, stride);
#endif
}


//---------------- init ordered -----------------------------------------
//----------- static
void xomp_loop_ordered_static_init(int* lower, int* upper, int* stride, int* chunk_size);
#pragma weak xomp_loop_ordered_static_init_=xomp_loop_ordered_static_init
void xomp_loop_ordered_static_init(int* lower, int* upper, int* stride, int* chunk_size)
{
  XOMP_loop_ordered_static_init (*lower, *upper, *stride, *chunk_size);
}
// scheduler initialization, only meaningful used for OMNI
void XOMP_loop_ordered_static_init(int lower, int upper, int stride, int chunk_size)
{
#ifdef USE_ROSE_GOMP_OPENMP_LIBRARY  
  // empty operation for gomp
#else   
  // adjust inclusive upper bounds of XOMP to non inclusive bounds of GOMP and OMNI
  if (stride>0)
    upper ++; //+1 to be non-inclusive for an incremental iteration space
  else
    upper --;
   // used for the ordered clause
   // void _ompc_init_ordered(int lb,int step)
   _ompc_init_ordered (lower, upper);
   if (chunk_size ==0)
     chunk_size =1 ; //TODO calculate the default static chunk size here
                      // Omni actually has another simpler way to support ordered default scheduling
                      // we use ordered static scheduling to have a uniform translation as the one supporting GOMP
   _ompc_static_sched_init (lower, upper, stride, chunk_size);
#endif    
}


//-----------dynamic
void xomp_loop_ordered_dynamic_init(int* lower, int* upper, int* stride, int* chunk_size);
#pragma weak xomp_loop_ordered_dynamic_init_=xomp_loop_ordered_dynamic_init
void xomp_loop_ordered_dynamic_init(int* lower, int* upper, int* stride, int* chunk_size)
{
  XOMP_loop_ordered_dynamic_init (*lower, *upper, *stride, *chunk_size);
}

// scheduler initialization, only meaningful used for OMNI
void XOMP_loop_ordered_dynamic_init(int lower, int upper, int stride, int chunk_size)
{
#ifdef USE_ROSE_GOMP_OPENMP_LIBRARY  
  // empty operation for gomp
#else   
  // adjust inclusive upper bounds of XOMP to non inclusive bounds of GOMP and OMNI
  if (stride>0)
    upper ++; //+1 to be non-inclusive for an incremental iteration space
  else
    upper --;

   // used for the ordered clause
   // void _ompc_init_ordered(int lb,int step)
   _ompc_init_ordered (lower, upper);
   if (chunk_size ==0)
     chunk_size =1 ; 
   _ompc_dynamic_sched_init (lower, upper, stride, chunk_size);
#endif    
}

//-----------guided
void xomp_loop_ordered_guided_init(int* lower, int* upper, int* stride, int* chunk_size);
#pragma weak xomp_loop_ordered_guided_init_=xomp_loop_ordered_guided_init
void xomp_loop_ordered_guided_init(int* lower, int* upper, int* stride, int* chunk_size)
{
  XOMP_loop_ordered_guided_init (*lower, *upper, *stride, *chunk_size);
}
// scheduler initialization, only meaningful used for OMNI
void XOMP_loop_ordered_guided_init(int lower, int upper, int stride, int chunk_size)
{
#ifdef USE_ROSE_GOMP_OPENMP_LIBRARY  
  // empty operation for gomp
#else   
  // adjust inclusive upper bounds of XOMP to non inclusive bounds of GOMP and OMNI
  if (stride>0)
    upper ++; //+1 to be non-inclusive for an incremental iteration space
  else
    upper --;

   // used for the ordered clause
   // void _ompc_init_ordered(int lb,int step)
   _ompc_init_ordered (lower, upper);
   if (chunk_size ==0)
     chunk_size =1 ; 
   _ompc_guided_sched_init (lower, upper, stride, chunk_size);
#endif    
}

//-----------runtime
void xomp_loop_ordered_runtime_init(int* lower, int* upper, int* stride);
#pragma weak xomp_loop_ordered_runtime_init_=xomp_loop_ordered_runtime_init
void xomp_loop_ordered_runtime_init(int* lower, int* upper, int* stride)
{
  XOMP_loop_ordered_runtime_init (*lower, *upper, *stride);
}
// scheduler initialization, only meaningful used for OMNI
void XOMP_loop_ordered_runtime_init(int lower, int upper, int stride)
{
#ifdef USE_ROSE_GOMP_OPENMP_LIBRARY  
  // empty operation for gomp
#else   
  // adjust inclusive upper bounds of XOMP to non inclusive bounds of GOMP and OMNI
  if (stride>0)
    upper ++; //+1 to be non-inclusive for an incremental iteration space
  else
    upper --;

   // used for the ordered clause
   // void _ompc_init_ordered(int lb,int step)
   _ompc_init_ordered (lower, upper);
   _ompc_runtime_sched_init (lower, upper, stride);
#endif    
}

//----------------------  start---------------------------------
// -----------  static
bool xomp_loop_static_start(int* start, int* end, int* incr, int* chunk_size,int *istart, int *iend);
#pragma weak xomp_loop_static_start_=xomp_loop_static_start
bool xomp_loop_static_start(int* start, int* end, int* incr, int* chunk_size,int *istart, int *iend)
{
  long l_istart, l_iend;
  bool rt; 
  rt = XOMP_loop_static_start (*start, *end, *incr, *chunk_size, &l_istart, &l_iend);
  if (rt)
  {
    CHECK_SIGNED_INT_RANGE(l_istart);
    CHECK_SIGNED_INT_RANGE(l_iend);
  }
  *istart = l_istart;
  *iend = l_iend;
//  printf ("debug: xomp_loop_static_start() start=%ld , end=%ld, rt =%d size is=%lu \n", l_istart, l_iend, rt,sizeof(rt));
  return rt;

}
bool XOMP_loop_static_start (long start, long end, long incr, long chunk_size,long *istart, long *iend)
{
  bool rt ;
  long lend;

// convert inclusive bounds of XOMP to non-inclusive upper bound from GOMP/OMNI
  if (incr>0 )
   end ++;
  else
   end --;
 
#ifdef USE_ROSE_GOMP_OPENMP_LIBRARY  
  rt = GOMP_loop_static_start (start, end, incr, chunk_size, istart, &lend);
#else   
  rt = _ompc_static_sched_next((int*)istart, (int*)(&lend));
#endif    

// convert non-inclusive upper bound from GOMP/OMNI to inclusive bounds of XOMP
  if (incr > 0)
   *iend = lend -1 ;
  else
   *iend = lend + 1;

  return rt; 
}
// -----------  dynamic
bool xomp_loop_dynamic_start(int* start, int* end, int* incr, int* chunk_size,int *istart, int *iend);
#pragma weak xomp_loop_dynamic_start_=xomp_loop_dynamic_start
bool xomp_loop_dynamic_start(int* start, int* end, int* incr, int* chunk_size,int *istart, int *iend)
{
  long l_istart, l_iend;
  bool rt;
  rt = XOMP_loop_dynamic_start (*start, *end, *incr, *chunk_size, &l_istart, &l_iend);
  if (rt)
  {
    CHECK_SIGNED_INT_RANGE(l_istart);
    CHECK_SIGNED_INT_RANGE(l_iend);
  }
  *istart = l_istart;
  *iend = l_iend;
//  printf ("debug: xomp_loop_dynamic_start() start=%ld , end=%ld, rt =%d size is=%lu \n", l_istart, l_iend, rt,sizeof(rt));
  return rt;

}
bool XOMP_loop_dynamic_start (long start, long end, long incr, long chunk_size,long *istart, long *iend)
{
  bool rt ;
  long lend;

// convert inclusive bounds of XOMP to non-inclusive upper bound from GOMP/OMNI
  if (incr>0 )
   end ++;
  else
   end --;

#ifdef USE_ROSE_GOMP_OPENMP_LIBRARY  
  rt = GOMP_loop_dynamic_start (start, end, incr, chunk_size, istart, &lend);
#else  
  rt = _ompc_dynamic_sched_next((int*)istart, (int*)(&lend));
#endif   

// convert non-inclusive upper bound from GOMP/OMNI to inclusive bounds of XOMP
  if (incr > 0)
   *iend = lend -1 ;
  else
   *iend = lend + 1;

  return rt;
}
// -----------  guided
bool xomp_loop_guided_start(int* start, int* end, int* incr, int* chunk_size,int *istart, int *iend);
#pragma weak xomp_loop_guided_start_=xomp_loop_guided_start
bool xomp_loop_guided_start(int* start, int* end, int* incr, int* chunk_size,int *istart, int *iend)
{
  long l_istart, l_iend;
  bool rt;
  rt = XOMP_loop_guided_start (*start, *end, *incr, *chunk_size, &l_istart, &l_iend);
  if (rt)
  {
    CHECK_SIGNED_INT_RANGE(l_istart);
    CHECK_SIGNED_INT_RANGE(l_iend);
  }
  *istart = l_istart;
  *iend = l_iend;
//  printf ("debug: xomp_loop_guided_start() start=%ld , end=%ld, rt =%d size is=%lu \n", l_istart, l_iend, rt,sizeof(rt));
  return rt;

}
bool XOMP_loop_guided_start (long start, long end, long incr, long chunk_size,long *istart, long *iend)
{
  bool rt ;
  long lend;

// convert inclusive bounds of XOMP to non-inclusive upper bound from GOMP/OMNI
  if (incr>0 )
   end ++;
  else
   end --;

#ifdef USE_ROSE_GOMP_OPENMP_LIBRARY  
  rt = GOMP_loop_guided_start (start, end, incr, chunk_size, istart, &lend);
#else  
  rt = _ompc_guided_sched_next((int*)istart, (int*)(&lend));
#endif   

// convert non-inclusive upper bound from GOMP/OMNI to inclusive bounds of XOMP
  if (incr > 0)
   *iend = lend -1 ;
  else
   *iend = lend + 1;

  return rt;
}
// -----------  runtime
bool xomp_loop_runtime_start(int* start, int* end, int* incr, int *istart, int *iend);
#pragma weak xomp_loop_runtime_start_=xomp_loop_runtime_start
bool xomp_loop_runtime_start(int* start, int* end, int* incr, int *istart, int *iend)
{
  long l_istart, l_iend;
  bool rt;
  rt = XOMP_loop_runtime_start (*start, *end, *incr,  &l_istart, &l_iend);
  if (rt)
  {
    CHECK_SIGNED_INT_RANGE(l_istart);
    CHECK_SIGNED_INT_RANGE(l_iend);
  }
  *istart = l_istart;
  *iend = l_iend;
//  printf ("debug: xomp_loop_runtime_start() start=%ld , end=%ld, rt =%d size is=%lu \n", l_istart, l_iend, rt,sizeof(rt));
  return rt;

}
bool XOMP_loop_runtime_start (long start, long end, long incr,long *istart, long *iend)
{
  bool rt ;
  long lend;

// convert inclusive bounds of XOMP to non-inclusive upper bound from GOMP/OMNI
  if (incr>0 )
   end ++;
  else
   end --;

#ifdef USE_ROSE_GOMP_OPENMP_LIBRARY  
  rt = GOMP_loop_runtime_start (start, end, incr, istart, &lend);
#else  
  rt = _ompc_runtime_sched_next((int*)istart, (int*)(&lend));
#endif   

// convert non-inclusive upper bound from GOMP/OMNI to inclusive bounds of XOMP
  if (incr > 0)
   *iend = lend -1 ;
  else
   *iend = lend + 1;

  return rt;
}

#if 0
//-------dynamic
bool XOMP_loop_dynamic_start (long start, long end, long incr, long chunk_size,long *istart, long *iend)
{
#ifdef USE_ROSE_GOMP_OPENMP_LIBRARY  
  return GOMP_loop_dynamic_start (start, end, incr, chunk_size, istart, iend);
#else   
  return _ompc_dynamic_sched_next((int*)istart, (int*) iend);
#endif    

}
//-------guided
bool XOMP_loop_guided_start (long start, long end, long incr, long chunk_size,long *istart, long *iend)
{
#ifdef USE_ROSE_GOMP_OPENMP_LIBRARY  
  return GOMP_loop_guided_start (start, end, incr, chunk_size, istart, iend);
#else   
  return _ompc_guided_sched_next((int*)istart, (int*)iend);
#endif    

}
//-------runtime
bool XOMP_loop_runtime_start (long start, long end, long incr, long *istart, long *iend)
{
#ifdef USE_ROSE_GOMP_OPENMP_LIBRARY  
  return GOMP_loop_runtime_start (start, end, incr, istart, iend);
#else   
  return _ompc_runtime_sched_next((int*)istart, (int*)iend);
#endif    

}
#endif
 //----------------------------------ordered start --------------------------
// -----------  ordered_static
bool xomp_loop_ordered_static_start(int* start, int* end, int* incr, int* chunk_size,int *istart, int *iend);
#pragma weak xomp_loop_ordered_static_start_=xomp_loop_ordered_static_start
bool xomp_loop_ordered_static_start(int* start, int* end, int* incr, int* chunk_size,int *istart, int *iend)
{
  long l_istart, l_iend;
  bool rt; 
  rt = XOMP_loop_ordered_static_start (*start, *end, *incr, *chunk_size, &l_istart, &l_iend);
  if (rt)
  {
    CHECK_SIGNED_INT_RANGE(l_istart);
    CHECK_SIGNED_INT_RANGE(l_iend);
  } 
  *istart = l_istart;
  *iend = l_iend;
//  printf ("debug: xomp_loop_ordered_static_start() start=%ld , end=%ld, rt =%d size is=%lu \n", l_istart, l_iend, rt,sizeof(rt));
  return rt;
  
}
bool XOMP_loop_ordered_static_start (long start, long end, long incr, long chunk_size,long *istart, long *iend)
{
  bool rt ;
  long lend;
  
// convert inclusive bounds of XOMP to non-inclusive upper bound from GOMP/OMNI
  if (incr>0 )
   end ++;
  else 
   end --;
   
#ifdef USE_ROSE_GOMP_OPENMP_LIBRARY  
  rt = GOMP_loop_ordered_static_start (start, end, incr, chunk_size, istart, &lend);
#else   
  rt = _ompc_static_sched_next((int*)istart, (int*)(&lend));
#endif    

// convert non-inclusive upper bound from GOMP/OMNI to inclusive bounds of XOMP
  if (incr > 0)
   *iend = lend -1 ;
  else
   *iend = lend + 1;
   
  return rt;
} 

// -----------  ordered_dynamic
bool xomp_loop_ordered_dynamic_start(int* start, int* end, int* incr, int* chunk_size,int *istart, int *iend);
#pragma weak xomp_loop_ordered_dynamic_start_=xomp_loop_ordered_dynamic_start
bool xomp_loop_ordered_dynamic_start(int* start, int* end, int* incr, int* chunk_size,int *istart, int *iend)
{
  long l_istart, l_iend;
  bool rt; 
  rt = XOMP_loop_ordered_dynamic_start (*start, *end, *incr, *chunk_size, &l_istart, &l_iend);
  if (rt)
  {
    CHECK_SIGNED_INT_RANGE(l_istart);
    CHECK_SIGNED_INT_RANGE(l_iend);
  } 
  *istart = l_istart;
  *iend = l_iend;
//  printf ("debug: xomp_loop_ordered_dynamic_start() start=%ld , end=%ld, rt =%d size is=%lu \n", l_istart, l_iend, rt,sizeof(rt));
  return rt;
  
}
bool XOMP_loop_ordered_dynamic_start (long start, long end, long incr, long chunk_size,long *istart, long *iend)
{
  bool rt ;
  long lend;
  
// convert inclusive bounds of XOMP to non-inclusive upper bound from GOMP/OMNI
  if (incr>0 )
   end ++;
  else 
   end --;
   
#ifdef USE_ROSE_GOMP_OPENMP_LIBRARY  
  rt = GOMP_loop_ordered_dynamic_start (start, end, incr, chunk_size, istart, &lend);
#else   
  rt = _ompc_dynamic_sched_next((int*)istart, (int*)(&lend));
#endif    

// convert non-inclusive upper bound from GOMP/OMNI to inclusive bounds of XOMP
  if (incr > 0)
   *iend = lend -1 ;
  else
   *iend = lend + 1;
   
  return rt;
} 

// -----------  ordered_guided
bool xomp_loop_ordered_guided_start(int* start, int* end, int* incr, int* chunk_size,int *istart, int *iend);
#pragma weak xomp_loop_ordered_guided_start_=xomp_loop_ordered_guided_start
bool xomp_loop_ordered_guided_start(int* start, int* end, int* incr, int* chunk_size,int *istart, int *iend)
{
  long l_istart, l_iend;
  bool rt; 
  rt = XOMP_loop_ordered_guided_start (*start, *end, *incr, *chunk_size, &l_istart, &l_iend);
  if (rt)
  {
    CHECK_SIGNED_INT_RANGE(l_istart);
    CHECK_SIGNED_INT_RANGE(l_iend);
  } 
  *istart = l_istart;
  *iend = l_iend;
//  printf ("debug: xomp_loop_ordered_guided_start() start=%ld , end=%ld, rt =%d size is=%lu \n", l_istart, l_iend, rt,sizeof(rt));
  return rt;
  
}
bool XOMP_loop_ordered_guided_start (long start, long end, long incr, long chunk_size,long *istart, long *iend)
{
  bool rt ;
  long lend;
  
// convert inclusive bounds of XOMP to non-inclusive upper bound from GOMP/OMNI
  if (incr>0 )
   end ++;
  else 
   end --;
   
#ifdef USE_ROSE_GOMP_OPENMP_LIBRARY  
  rt = GOMP_loop_ordered_guided_start (start, end, incr, chunk_size, istart, &lend);
#else   
  rt = _ompc_guided_sched_next((int*)istart, (int*)(&lend));
#endif    

// convert non-inclusive upper bound from GOMP/OMNI to inclusive bounds of XOMP
  if (incr > 0)
   *iend = lend -1 ;
  else
   *iend = lend + 1;
   
  return rt;
} 
// -----------  ordered_runtime
bool xomp_loop_ordered_runtime_start(int* start, int* end, int* incr, int *istart, int *iend);
#pragma weak xomp_loop_ordered_runtime_start_=xomp_loop_ordered_runtime_start
bool xomp_loop_ordered_runtime_start(int* start, int* end, int* incr, int *istart, int *iend)
{
  long l_istart, l_iend;
  bool rt;
  rt = XOMP_loop_ordered_runtime_start (*start, *end, *incr,  &l_istart, &l_iend);
  if (rt)
  {
    CHECK_SIGNED_INT_RANGE(l_istart);
    CHECK_SIGNED_INT_RANGE(l_iend);
  }
  *istart = l_istart;
  *iend = l_iend;
//  printf ("debug: xomp_loop_ordered_runtime_start() start=%ld , end=%ld, rt =%d size is=%lu \n", l_istart, l_iend, rt,sizeof(rt));
  return rt;

}
bool XOMP_loop_ordered_runtime_start (long start, long end, long incr,long *istart, long *iend)
{
  bool rt ;
  long lend;

// convert inclusive bounds of XOMP to non-inclusive upper bound from GOMP/OMNI
  if (incr>0 )
   end ++;
  else
   end --;

#ifdef USE_ROSE_GOMP_OPENMP_LIBRARY  
  rt = GOMP_loop_ordered_runtime_start (start, end, incr, istart, &lend);
#else
#endif

// convert non-inclusive upper bound from GOMP/OMNI to inclusive bounds of XOMP
  if (incr > 0)
   *iend = lend -1 ;
  else
   *iend = lend + 1;

  return rt;
}

#if 0
//-------static
bool XOMP_loop_ordered_static_start (long start, long end, long incr, long chunk_size,long *istart, long *iend)
{
#ifdef USE_ROSE_GOMP_OPENMP_LIBRARY  
  return GOMP_loop_ordered_static_start (start, end, incr, chunk_size, istart, iend);
#else   
  return _ompc_static_sched_next ((int*)istart, (int*)iend); 
#endif    
}

//-------dynamic
bool XOMP_loop_ordered_dynamic_start (long start, long end, long incr, long chunk_size,long *istart, long *iend)
{
#ifdef USE_ROSE_GOMP_OPENMP_LIBRARY  
  return GOMP_loop_ordered_dynamic_start (start, end, incr, chunk_size, istart, iend);
#else   
  return _ompc_dynamic_sched_next ((int*)istart, (int*)iend); 
#endif    
}

//-------guided
bool XOMP_loop_ordered_guided_start (long start, long end, long incr, long chunk_size,long *istart, long *iend)
{
#ifdef USE_ROSE_GOMP_OPENMP_LIBRARY  
  return GOMP_loop_ordered_guided_start (start, end, incr, chunk_size, istart, iend);
#else   
  return _ompc_guided_sched_next ((int*)istart, (int*)iend); 
#endif    
}

//-------runtime
bool XOMP_loop_ordered_runtime_start (long start, long end, long incr, long *istart, long *iend)
{
#ifdef USE_ROSE_GOMP_OPENMP_LIBRARY  
  return GOMP_loop_ordered_runtime_start (start, end, incr, istart, iend);
#else   
#endif    
}
#endif

//---------------------------- loop next------------------------------------------
//----- static--
bool xomp_loop_static_next(int* l, int * u);
#pragma weak xomp_loop_static_next_=xomp_loop_static_next
bool xomp_loop_static_next(int* l, int * u)
{
  long l_l, l_u;
  bool rt; 
  rt = XOMP_loop_static_next(&l_l, &l_u);
  if (rt)
  { // avoid assertion on garbage values
    CHECK_SIGNED_INT_RANGE(l_l);
    CHECK_SIGNED_INT_RANGE(l_u);
  }
  *l= l_l;
  *u= l_u;
  return rt; 
}
bool XOMP_loop_static_next (long * l, long *u)
{
  bool rt;
  long lu;
#ifdef USE_ROSE_GOMP_OPENMP_LIBRARY  
  rt = GOMP_loop_static_next (l, &lu);
#else   
   rt = _ompc_static_sched_next((int*)l, (int*)(&lu));
#endif    
  if (*l< lu)
    *u = lu -1;
  else
    *u = lu +1;
   return rt;
}
//----- dynamic--
bool xomp_loop_dynamic_next(int* l, int * u);
#pragma weak xomp_loop_dynamic_next_=xomp_loop_dynamic_next
bool xomp_loop_dynamic_next(int* l, int * u)
{
  long l_l, l_u;
  bool rt;
  rt = XOMP_loop_dynamic_next(&l_l, &l_u);
  if (rt)
  { // avoid assertion on garbage values
    CHECK_SIGNED_INT_RANGE(l_l);
    CHECK_SIGNED_INT_RANGE(l_u);
  }
  *l= l_l;
  *u= l_u;
  return rt;
}
bool XOMP_loop_dynamic_next (long * l, long *u)
{
  bool rt;
  long lu;
#ifdef USE_ROSE_GOMP_OPENMP_LIBRARY  
  rt = GOMP_loop_dynamic_next (l, &lu);
#else
   rt = _ompc_dynamic_sched_next((int*)l, (int*)(&lu));
#endif
  if (*l< lu)
    *u = lu -1;
  else 
    *u = lu +1;
   return rt;
}

//----- guided--
bool xomp_loop_guided_next(int* l, int * u);
#pragma weak xomp_loop_guided_next_=xomp_loop_guided_next
bool xomp_loop_guided_next(int* l, int * u)
{
  long l_l, l_u;
  bool rt;
  rt = XOMP_loop_guided_next(&l_l, &l_u);
  if (rt)
  { // avoid assertion on garbage values
    CHECK_SIGNED_INT_RANGE(l_l);
    CHECK_SIGNED_INT_RANGE(l_u);
  }
  *l= l_l;
  *u= l_u;
  return rt;
}
bool XOMP_loop_guided_next (long * l, long *u)
{
  bool rt;
  long lu;
#ifdef USE_ROSE_GOMP_OPENMP_LIBRARY  
  rt = GOMP_loop_guided_next (l, &lu);
#else
   rt = _ompc_guided_sched_next((int*)l, (int*)(&lu));
#endif
  if (*l< lu)
    *u = lu -1;
  else 
    *u = lu +1;
   return rt;
}

//----- runtime--
bool xomp_loop_runtime_next(int* l, int * u);
#pragma weak xomp_loop_runtime_next_=xomp_loop_runtime_next
bool xomp_loop_runtime_next(int* l, int * u)
{
  long l_l, l_u;
  bool rt;
  rt = XOMP_loop_runtime_next(&l_l, &l_u);
  if (rt)
  { // avoid assertion on garbage values
    CHECK_SIGNED_INT_RANGE(l_l);
    CHECK_SIGNED_INT_RANGE(l_u);
  }
  *l= l_l;
  *u= l_u;
  return rt;
}
bool XOMP_loop_runtime_next (long * l, long *u)
{
  bool rt;
  long lu;
#ifdef USE_ROSE_GOMP_OPENMP_LIBRARY  
  rt = GOMP_loop_runtime_next (l, &lu);
#else
   rt = _ompc_runtime_sched_next((int*)l, (int*)(&lu));
#endif
  if (*l< lu)
    *u = lu -1;
  else 
    *u = lu +1;
   return rt;
}

#if 0
//----- dynamic--
bool XOMP_loop_dynamic_next (long *l, long *u)
{
#ifdef USE_ROSE_GOMP_OPENMP_LIBRARY  
  return GOMP_loop_dynamic_next (l, u);
#else   
   return _ompc_dynamic_sched_next((int*)l, (int*)u);
#endif    
}


//----- guided--
bool XOMP_loop_guided_next (long *l, long *u)
{
#ifdef USE_ROSE_GOMP_OPENMP_LIBRARY  
  return GOMP_loop_guided_next (l, u);
#else   
   return _ompc_guided_sched_next((int *)l,(int *) u);
#endif    
}


//----- runtime--
bool XOMP_loop_runtime_next (long *l, long *u)
{
#ifdef USE_ROSE_GOMP_OPENMP_LIBRARY  
  return GOMP_loop_runtime_next (l, u);
#else   
   return _ompc_runtime_sched_next((int *)l, (int *)u);
#endif    
}
#endif

//---------------------------- loop ordered next------------------------------------------
//----- ordered_static--
bool xomp_loop_ordered_static_next(int* l, int * u);
#pragma weak xomp_loop_ordered_static_next_=xomp_loop_ordered_static_next
bool xomp_loop_ordered_static_next(int* l, int * u)
{
  long l_l, l_u;
  bool rt;
  rt = XOMP_loop_ordered_static_next(&l_l, &l_u);
  if (rt)
  { // avoid assertion on garbage values
    CHECK_SIGNED_INT_RANGE(l_l);
    CHECK_SIGNED_INT_RANGE(l_u);
  }
  *l= l_l;
  *u= l_u;
  return rt;
}
bool XOMP_loop_ordered_static_next (long * l, long *u)
{
  bool rt;
  long lu;
#ifdef USE_ROSE_GOMP_OPENMP_LIBRARY  
  rt = GOMP_loop_ordered_static_next (l, &lu);
#else
   rt = _ompc_static_sched_next((int*)l, (int*)(&lu));
#endif
  if (*l< lu)
    *u = lu -1;
  else 
    *u = lu +1;
   return rt;
}
//----- ordered_dynamic--
bool xomp_loop_ordered_dynamic_next(int* l, int * u);
#pragma weak xomp_loop_ordered_dynamic_next_=xomp_loop_ordered_dynamic_next
bool xomp_loop_ordered_dynamic_next(int* l, int * u)
{
  long l_l, l_u;
  bool rt;
  rt = XOMP_loop_ordered_dynamic_next(&l_l, &l_u);
  if (rt)
  { // avoid assertion on garbage values
    CHECK_SIGNED_INT_RANGE(l_l);
    CHECK_SIGNED_INT_RANGE(l_u);
  }
  *l= l_l;
  *u= l_u;
  return rt;
}
bool XOMP_loop_ordered_dynamic_next (long * l, long *u)
{
  bool rt;
  long lu;
#ifdef USE_ROSE_GOMP_OPENMP_LIBRARY  
  rt = GOMP_loop_ordered_dynamic_next (l, &lu);
#else
   rt = _ompc_dynamic_sched_next((int*)l, (int*)(&lu));
#endif
  if (*l< lu)
    *u = lu -1;
  else 
    *u = lu +1;
   return rt;
}

//----- ordered_guided--
bool xomp_loop_ordered_guided_next(int* l, int * u);
#pragma weak xomp_loop_ordered_guided_next_=xomp_loop_ordered_guided_next
bool xomp_loop_ordered_guided_next(int* l, int * u)
{
  long l_l, l_u;
  bool rt;
  rt = XOMP_loop_ordered_guided_next(&l_l, &l_u);
  if (rt)
  { // avoid assertion on garbage values
    CHECK_SIGNED_INT_RANGE(l_l);
    CHECK_SIGNED_INT_RANGE(l_u);
  }
  *l= l_l;
  *u= l_u;
  return rt;
}
bool XOMP_loop_ordered_guided_next (long * l, long *u)
{
  bool rt;
  long lu;
#ifdef USE_ROSE_GOMP_OPENMP_LIBRARY  
  rt = GOMP_loop_ordered_guided_next (l, &lu);
#else
   rt = _ompc_guided_sched_next((int*)l, (int*)(&lu));
#endif
  if (*l< lu)
    *u = lu -1;
  else 
    *u = lu +1;
   return rt;
}

//----- ordered_runtime--
bool xomp_loop_ordered_runtime_next(int* l, int * u);
#pragma weak xomp_loop_ordered_runtime_next_=xomp_loop_ordered_runtime_next
bool xomp_loop_ordered_runtime_next(int* l, int * u)
{
  long l_l, l_u;
  bool rt;
  rt = XOMP_loop_ordered_runtime_next(&l_l, &l_u);
  if (rt)
  { // avoid assertion on garbage values
    CHECK_SIGNED_INT_RANGE(l_l);
    CHECK_SIGNED_INT_RANGE(l_u);
  }
  *l= l_l;
  *u= l_u;
  return rt;
}
bool XOMP_loop_ordered_runtime_next (long * l, long *u)
{
  bool rt;
  long lu;
#ifdef USE_ROSE_GOMP_OPENMP_LIBRARY  
  rt = GOMP_loop_ordered_runtime_next (l, &lu);
#else
   rt = _ompc_runtime_sched_next((int*)l, (int*)(&lu));
#endif
  if (*l< lu)
    *u = lu -1;
  else 
    *u = lu +1;
   return rt;
}

#if 0
//------static
bool XOMP_loop_ordered_static_next (long *a, long * b)
{
#ifdef USE_ROSE_GOMP_OPENMP_LIBRARY  
  bool rt = GOMP_loop_ordered_static_next (a, b);
//  printf ("debug xomp: a =%d, b = %d \n",*a, *b);
  return rt;
#else 
  return _ompc_static_sched_next ((int *)a,(int *)b);
#endif    
}

//------dynamic
bool XOMP_loop_ordered_dynamic_next (long * a, long * b)
{
#ifdef USE_ROSE_GOMP_OPENMP_LIBRARY  
 return GOMP_loop_ordered_dynamic_next (a, b);
#else   
  return _ompc_dynamic_sched_next ((int *)a,(int *)b);
#endif    
}
//------guided
bool XOMP_loop_ordered_guided_next (long *a, long *b)
{
#ifdef USE_ROSE_GOMP_OPENMP_LIBRARY  
 return GOMP_loop_ordered_guided_next (a, b);
#else   
  return _ompc_guided_sched_next ((int *)a,(int *)b);
#endif    
}
//------runtime
bool XOMP_loop_ordered_runtime_next (long *a, long *b)
{
#ifdef USE_ROSE_GOMP_OPENMP_LIBRARY  
 return GOMP_loop_ordered_runtime_next (a, b);
#else   
  return _ompc_runtime_sched_next ((int*)a,(int*)b);
#endif    
}
#endif
//---------------  others---------------------------------------
void xomp_loop_end(void);
#pragma weak xomp_loop_end_=xomp_loop_end
void xomp_loop_end(void)
{
  XOMP_loop_end();
}
void XOMP_loop_end (void)
{
#ifdef USE_ROSE_GOMP_OPENMP_LIBRARY  
  GOMP_loop_end();
#else   
#endif    
}
//---------
void xomp_loop_end_nowait(void);
#pragma weak xomp_loop_end_nowait_=xomp_loop_end_nowait
void xomp_loop_end_nowait(void)
{
  XOMP_loop_end_nowait();
}

void XOMP_loop_end_nowait (void)
{
#ifdef USE_ROSE_GOMP_OPENMP_LIBRARY  
  GOMP_loop_end_nowait();
#else   
#endif    
}

//---------
void xomp_barrier(void);
#pragma weak  xomp_barrier_=xomp_barrier
void xomp_barrier(void)
{
  XOMP_barrier();
}
void XOMP_barrier (void)
{
#ifdef USE_ROSE_GOMP_OPENMP_LIBRARY  
  GOMP_barrier();
#else   
  _ompc_barrier();
#endif    

  //  else
  //  {
  //    printf("Error: un recognized runtime library type!");
  //    assert (false);
  //   } 
}

// named and unnamed critical
// GOMP have a dedicated function to support unnamed critical
// We generate a default name for it and use the named critical support function instead to
// be consistent with OMNI
void XOMP_critical_start (void** data)
{
#ifdef USE_ROSE_GOMP_OPENMP_LIBRARY  
    GOMP_critical_name_start(data);
#else   
    _ompc_enter_critical(data);
#endif    
}

void XOMP_critical_end (void** data)
{
#ifdef USE_ROSE_GOMP_OPENMP_LIBRARY  
    GOMP_critical_name_end(data);
#else   
    _ompc_exit_critical(data);
#endif    
}
//---------
bool xomp_single(void);
#pragma weak xomp_single_=xomp_single
bool xomp_single(void)
{
  return XOMP_single();
}
extern bool XOMP_single(void)
{
#ifdef USE_ROSE_GOMP_OPENMP_LIBRARY  
  return GOMP_single_start();
#else   
  return _ompc_do_single();
#endif    
}
//---------
bool xomp_master(void);
#pragma weak xomp_master_=xomp_master
bool xomp_master(void)
{
  return XOMP_master();
}

extern bool XOMP_master(void)
{
#ifdef USE_ROSE_GOMP_OPENMP_LIBRARY  
  return (omp_get_thread_num() ==0);
#else   
  return _ompc_is_master ();
#endif    
}

//---------
void xomp_atomic_start(void);
#pragma weak xomp_atomic_start_=xomp_atomic_start
void xomp_atomic_start(void)
{
  XOMP_atomic_start();
}
void XOMP_atomic_start (void)
{
#ifdef USE_ROSE_GOMP_OPENMP_LIBRARY  
  GOMP_atomic_start();
#else   
  _ompc_atomic_lock();
#endif
}

//---------
void xomp_atomic_end(void);
#pragma weak xomp_atomic_end_=xomp_atomic_end
void xomp_atomic_end(void)
{
  XOMP_atomic_end();
}
void XOMP_atomic_end (void)
{
#ifdef USE_ROSE_GOMP_OPENMP_LIBRARY  
   GOMP_atomic_end();
#else   
  _ompc_atomic_unlock();
#endif
}

void XOMP_flush_all ()
{
#ifdef USE_ROSE_GOMP_OPENMP_LIBRARY  
  __sync_synchronize();
#else   
  _ompc_flush(0,0);
#endif
}

void XOMP_flush_one(char * startAddress, int nbyte)
{
#ifdef USE_ROSE_GOMP_OPENMP_LIBRARY  
  __sync_synchronize();
#else   
  _ompc_flush(startAddress,nbyte);
#endif
}

void XOMP_ordered_start (void)
{
#ifdef USE_ROSE_GOMP_OPENMP_LIBRARY  
   GOMP_ordered_start();
#else   
#endif

}
void XOMP_ordered_end (void)
{
#ifdef USE_ROSE_GOMP_OPENMP_LIBRARY  
  GOMP_ordered_end();
#else   

#endif

}

