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
#include <stdio.h> // for getenv(), file
#include <assert.h>
#include <stdarg.h>
#include <string.h> // for memcpy()

/* Timing support, Liao 2/15/2013 */
#include <sys/time.h>
#include <time.h> /*current time*/

int env_region_instr_val = 0;
FILE* fp = 0;

extern char* current_time_to_str(void);
char* current_time_to_str(void)
{
  /* careful about the size: match sprintf () */
  char *timestamp = (char *)malloc(sizeof(char) * 20);
  time_t ltime;
  ltime=time(NULL);
  struct tm *tm;
  tm=localtime(&ltime);

  assert(timestamp != NULL);
  assert(tm != NULL);
  sprintf(timestamp,"%04d_%02d_%02d_%02d_%02d_%02d", tm->tm_year+1900, tm->tm_mon+1, // month starts from 0
      tm->tm_mday, tm->tm_hour, tm->tm_min, tm->tm_sec);
  return timestamp;
}

double xomp_time_stamp(void)
{
  struct timeval t;
  double time;
//  static double prev_time=0.0;

  gettimeofday(&t, NULL);
  time = t.tv_sec + 1.0e-6*t.tv_usec;
  return time;
}

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
  char* env_var_str;
  int  env_var_val;
  env_var_str = getenv("XOMP_REGION_INSTR");
  if (env_var_str != NULL)
  {
    sscanf(env_var_str, "%d", &env_var_val);
    assert (env_var_val==0 || env_var_val == 1);
    env_region_instr_val = env_var_val;
  }

  if (env_region_instr_val)
  {
    char* instr_file_name;
    instr_file_name= current_time_to_str();
    assert(instr_file_name != NULL);
    fp = fopen(instr_file_name, "a+");
    if (fp != NULL)
    {
      printf("XOMP region instrumentation is turned on ...\n");
      fprintf (fp, "%f\t1\n",xomp_time_stamp());
    }
  }
#ifdef USE_ROSE_GOMP_OPENMP_LIBRARY  
#else   
  _ompc_init (argc, argv);
#endif    
}

void xomp_terminate (int* exitcode);
#pragma weak xomp_terminate_=xomp_terminate
void xomp_terminate (int* exitcode)
{
  XOMP_terminate (*exitcode);
}
// Runtime library termination routine
void XOMP_terminate (int exitcode)
{
  if (env_region_instr_val)
  {
    fprintf (fp, "%f\t1\n",xomp_time_stamp());
    fclose(fp);
  }
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

void xomp_parallel_start (void (*func) (void *), unsigned* ifClauseValue, unsigned* numThread, int * argcount, ...);
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
void xomp_parallel_start (void (*func) (void *), unsigned* ifClauseValue, unsigned* numThread, int * argcount, ...)
{
  int x;
  va_list v1;
  int p_offset=1; // the first position is reserved to the function pointer to the outlined Fortran subroutine

  // simplest case where no shared variables to be passed.
  if (*argcount == 0)
  {

    XOMP_parallel_start (func, 0, *ifClauseValue, *numThread, NULL, 0);
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

//  printf("Debug: xomp_parallel_start(), g_parameter at address %p\n", g_parameter);
  g_parameter[0]= func;
  //va_start (v1, (*argcount));
  va_start (v1, argcount);
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

void XOMP_parallel_start (void (*func) (void *), void *data, unsigned ifClauseValue, unsigned numThreadsSpecified, char* file_name, int line_no)
{
  if (env_region_instr_val)
  {
    fprintf (fp,"%f\t1\t%s\t%d\n",xomp_time_stamp(),file_name, line_no);
    fprintf (fp, "%f\t2\t%s\t%d\n",xomp_time_stamp(),file_name, line_no);
  }
#ifdef USE_ROSE_GOMP_OPENMP_LIBRARY 
  // XOMP  to GOMP
  unsigned numThread = 0;
  //numThread is 1 if an IF clause is present and false, 
  //          or the value of the NUM_THREADS clause, if present, or 0. 
  // It is only used to indicate a combination of the NUM_THREADS clause and the IF clause. 
  // GOMP has an internal function (gomp_resolve_num_threads()) to determine the actual number of threads to be used later on. 
  if (!ifClauseValue)
    numThread = 1;
  else
    numThread = numThreadsSpecified;

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
  XOMP_parallel_end (NULL, 0);
}
void XOMP_parallel_end (char* file_name, int line_no)
{
  //printf ("%s %f\n",__PRETTY_FUNCTION__, xomp_time_stamp());
  if (env_region_instr_val)
  {
    fprintf (fp,"%f\t2\t%s\t%d\n",xomp_time_stamp(),file_name, line_no);
    fprintf (fp, "%f\t1\t%s\t%d\n",xomp_time_stamp(),file_name, line_no);
  }
#ifdef USE_ROSE_GOMP_OPENMP_LIBRARY  
  GOMP_parallel_end ();
#else   
#endif    
}


//---------------------------------------------
//Glue from Fortran to XOMP
int xomp_sections_init_next(int * section_count);
#pragma weak xomp_sections_init_next_=xomp_sections_init_next
int xomp_sections_init_next(int * section_count)
{
  return XOMP_sections_init_next (*section_count);
}
// C/C++ support
/* Initialize sections and return the next section id (starting from 0) to be executed by the current thread */
int XOMP_sections_init_next(int section_count) 
{
  int result = -1;
#ifdef USE_ROSE_GOMP_OPENMP_LIBRARY  
  result = GOMP_sections_start (section_count);
  result --; /*GOMP sections start with 1*/
#else
  _ompc_section_init (section_count);
  result = _ompc_section_id();
  /* Omni expects sections to start with 0*/
#endif
  return result;
}

/* Return the next section id (starting from 1) to be executed by the current thread, value 0 means no sections left */
int xomp_sections_next(void);
#pragma weak xomp_sections_next_=xomp_sections_next
int xomp_sections_next(void)
{
  return XOMP_sections_next();
}
int XOMP_sections_next(void)
{
  int result = -1;
#ifdef USE_ROSE_GOMP_OPENMP_LIBRARY  
 result = GOMP_sections_next();
 result --;  /*GOMP sections start with 1*/
#else
  result = _ompc_section_id();
#endif
  return result;
}

void xomp_sections_end(void);
#pragma weak xomp_sections_end_=xomp_sections_end
void xomp_sections_end(void)
{
  XOMP_sections_end();
}
/* Called after the current thread is told that all sections are executed. It synchronizes all threads also. */
void XOMP_sections_end(void)
{
#ifdef USE_ROSE_GOMP_OPENMP_LIBRARY  
  GOMP_sections_end();
#else
#endif
}

void xomp_sections_end_nowait(void);
#pragma weak xomp_sections_end_nowait_=xomp_sections_end_nowait
void xomp_sections_end_nowait(void)
{
  XOMP_sections_end_nowait();
}
/* Called after the current thread is told that all sections are executed. It does not synchronizes all threads. */
void XOMP_sections_end_nowait(void)
{
#ifdef USE_ROSE_GOMP_OPENMP_LIBRARY  
  GOMP_sections_end_nowait();
#else
#endif
}

//---------------------------------------------
#include "run_me_task_defs.inc"
// statically allocated pg_parameter
#define MAX_XOMP_TASK_NUMBER 99999 // cannot be too large, otherwise exceed static allocation limit
void *task_parameter[MAX_XOMP_TASK_NUMBER][MAX_OUTLINED_FUNC_PARAMETER_COUNT];
void xomp_task(void (*func) (void *), void (*cpyfn) (void *, void *), int* arg_size, int* arg_align, 
               int* if_clause, int* untied, int * argcount, ...);
#pragma weak xomp_task_=xomp_task
// Main tasks: 
//   Convert Fortran parameters to the right C ones so XOMP_task() can handle them properly
//     The challenge is that Fortran parameters are all passed by references.
//     But XOMP_task() has to support both pass-by-reference and pass-by-value. 
//     Especially, the values/addresses must be copied into task data when tasks are created. 
//     Since a task's parent function stack may not exist when a task is being executed. 
//
//  Solution: 
//   Compiler provides all information about the nature (value vs. reference) of parameter, size, and where to get the parameter.
//   The XOMP layer encodes all information into a single data segment, which will be copied into each task by gomp_task() etc. 
//   The XOMP layer also provides function (run_me_task_ ()) to decode the data segment, and call the outlined Fortran subroutine. 
//
//   Wrap function pointer to a outlined function routine and all its parameters into one single parameter gp_parameter[]
//   The single data segment has the following layout
//        8 bytes for the function pointer to the outlined Fortran subroutine
//        a triplet for each parameter. Every triplet has the following layout
//          1 byte for a boolean value to indicate if the parameter should be passed by value
//          4 bytes for the size of the parameter, size should be equal to sizeof (void*) if not pass-by-value ( pointer type instead)
//          x bytes for value or address, x will be the size provide by the 2nd element of the triplet.
//  For example, if we have a outlined function with two parameters: 
//     an integer index suitable for pass-by-value, This is very typical for a task's firstprivate variables.
//     an array pass-by-reference, 
//  The data segment to be created will contain (assuming 64-bit target):
//      8 bytes (function pointer), 
//      1 byte(byValue=true),  4 bytes (int value),           8 bytes(address of the index)
//      1 byte(byValue=false), 8 bytes (address of the array), 8 bytes(address of the array)
//  Total   38 bytes for the data segment
//
//  Finally, we can call the real xomp_task () with the bridge run_me_task_#() function
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
  //va_start (v1, (*argcount));
  va_start (v1, argcount);

  // 2nd parameter and after ----------------
  // Fill in the actual data parameters for the function pointer
  for (x=0; x<largcount; x++)
  { 
     // grab 1st of the triplet: pass-by-value or not
    int by_value = *(int*)(va_arg(v1, void*));
    bool bValue = (by_value==1)?true:false;
    memcpy (&(pg_parameter[p_offset]),&bValue, sizeof(bool)); // we use 1 byte to store the by-value flag
    p_offset+= sizeof(bool);

     // grab 2nd of the triplet: size of the value/address
    int v_size =  *(int*)(va_arg(v1, void*));
    if (!by_value)
    { // verify size of address
      assert (v_size == sizeof(void*));
    }
    memcpy (&(pg_parameter[p_offset]),&v_size, sizeof(int)); // we use 4 byte to store the size
    p_offset+= sizeof(int);

    // 3rd of the triplet: address (of the value) : store value or address as required 
    if (by_value)
    {
      char * i = (char*)(va_arg(v1, void*)); // every parameter is a pointer to something from Fortran to xomp
      // in this case, the pointer points to a value, we just copy the pointed value bytes 
      memcpy (&(pg_parameter[p_offset]),i, v_size);
    }  
    else
    {
      // address
      void * i2 = (void*) (va_arg(v1, void*)); 
      memcpy (&(pg_parameter[p_offset]),&i2, v_size);
#if 0      
      printf ("Debug, fortran xomp, source array address is %p\n", i2);
      // debug the array copied
      int* item = (int*) (i2);
      int ii;
      for (ii = 0; ii<10; ii++)
        printf ("source item [%d]=%d\n", ii, item[ii]);
#endif        
    }
    p_offset+=v_size; // shift offset
  }

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
// loop scheduling 
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
// DQ (12/10/2016): We should discuss this since it is not initialized when USE_ROSE_GOMP_OPENMP_LIBRARY is not defined.
// bool rt ;
  bool rt = false;

// DQ (12/10/2016): Eliminating a warning that we want to be an error: -Werror=maybe-uninitialized.
// long lend;
  long lend = 0;

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
extern void XOMP_static_even_divide (long start, long orig_size, int chunk_count, int chunk_id, long * chunk_offset, long * chunk_size) 
{
//  long n = end - start + 1; // inclusive end bound, 0 to 9
  assert (orig_size>0);
  assert (chunk_count>0);
  assert (chunk_id>=0);
  long size = orig_size / chunk_count;
  long offset = start + size * chunk_id;
  long remainder = orig_size % chunk_count;
  if(chunk_id < remainder)
  {
    size++;
  }

  if(chunk_id >= remainder)
    offset += remainder;  // all extra elements on top of base line offset size*tid
  else
    offset += chunk_id;   // only partial extra elements   

  *chunk_offset = offset;
//  *n_upper = offset +size -1;
  *chunk_size = size; 
}

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
