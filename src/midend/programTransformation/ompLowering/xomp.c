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

#include <stdlib.h> // for getenv(), malloc(), etc
#include <stdio.h> // for getenv()
#include <assert.h>
#include <stdarg.h>

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

//Runtime library initialization routine
void XOMP_init (int argc, char ** argv)
{
#ifdef USE_ROSE_GOMP_OPENMP_LIBRARY  
#else   
  _ompc_init (argc, argv);
#endif    
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
void *g_parameter[MAX_OUTLINED_FUNC_PARAMETER_COUNT];
// void **g_parameter;

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

  g_parameter[0]= func;
  va_start (v1, (*argcount));
  for (x=0; x<(*argcount); x++)
  {
    g_parameter[p_offset+x]= va_arg(v1, void*);
  }

  switch (*argcount)
  {
//   XOMP_parallel_start (run_me, (void*) g_parameter, *numThread);
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

// scheduler initialization, only meaningful used for OMNI
void XOMP_loop_static_init(int lower, int upper, int stride, int chunk_size)
{
#ifdef USE_ROSE_GOMP_OPENMP_LIBRARY  
  // empty operation for gomp
#else   
   _ompc_static_sched_init (lower, upper, stride, chunk_size);
#endif    
}

// scheduler initialization, only meaningful used for OMNI
void XOMP_loop_ordered_static_init(int lower, int upper, int stride, int chunk_size)
{
#ifdef USE_ROSE_GOMP_OPENMP_LIBRARY  
  // empty operation for gomp
#else   
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


void XOMP_loop_dynamic_init(int lower, int upper, int stride, int chunk_size)
{
#ifdef USE_ROSE_GOMP_OPENMP_LIBRARY  
#else   
  _ompc_dynamic_sched_init (lower, upper, stride, chunk_size);
#endif    
}

// scheduler initialization, only meaningful used for OMNI
void XOMP_loop_ordered_dynamic_init(int lower, int upper, int stride, int chunk_size)
{
#ifdef USE_ROSE_GOMP_OPENMP_LIBRARY  
  // empty operation for gomp
#else   
   // used for the ordered clause
   // void _ompc_init_ordered(int lb,int step)
   _ompc_init_ordered (lower, upper);
   if (chunk_size ==0)
     chunk_size =1 ; 
   _ompc_dynamic_sched_init (lower, upper, stride, chunk_size);
#endif    
}


void XOMP_loop_guided_init(int lower, int upper, int stride, int chunk_size)
{

#ifdef USE_ROSE_GOMP_OPENMP_LIBRARY  
#else   
  _ompc_guided_sched_init (lower, upper, stride, chunk_size);
#endif    
}
// scheduler initialization, only meaningful used for OMNI
void XOMP_loop_ordered_guided_init(int lower, int upper, int stride, int chunk_size)
{
#ifdef USE_ROSE_GOMP_OPENMP_LIBRARY  
  // empty operation for gomp
#else   
   // used for the ordered clause
   // void _ompc_init_ordered(int lb,int step)
   _ompc_init_ordered (lower, upper);
   if (chunk_size ==0)
     chunk_size =1 ; 
   _ompc_guided_sched_init (lower, upper, stride, chunk_size);
#endif    
}


void XOMP_loop_runtime_init(int lower, int upper, int stride)
{
#ifdef USE_ROSE_GOMP_OPENMP_LIBRARY  
#else   
  _ompc_runtime_sched_init (lower, upper, stride);
#endif    
}
// scheduler initialization, only meaningful used for OMNI
void XOMP_loop_ordered_runtime_init(int lower, int upper, int stride)
{
#ifdef USE_ROSE_GOMP_OPENMP_LIBRARY  
  // empty operation for gomp
#else   
   // used for the ordered clause
   // void _ompc_init_ordered(int lb,int step)
   _ompc_init_ordered (lower, upper);
   _ompc_runtime_sched_init (lower, upper, stride);
#endif    
}


// if (start), 
bool XOMP_loop_static_start (long start, long end, long incr, long chunk_size,long *istart, long *iend)
{
#ifdef USE_ROSE_GOMP_OPENMP_LIBRARY  
  return GOMP_loop_static_start (start, end, incr, chunk_size, istart, iend);
#else   
  return _ompc_static_sched_next((int*)istart, (int*)iend);
#endif    
}

bool XOMP_loop_dynamic_start (long start, long end, long incr, long chunk_size,long *istart, long *iend)
{
#ifdef USE_ROSE_GOMP_OPENMP_LIBRARY  
  return GOMP_loop_dynamic_start (start, end, incr, chunk_size, istart, iend);
#else   
  return _ompc_dynamic_sched_next((int*)istart, (int*) iend);
#endif    

}
bool XOMP_loop_guided_start (long start, long end, long incr, long chunk_size,long *istart, long *iend)
{
#ifdef USE_ROSE_GOMP_OPENMP_LIBRARY  
  return GOMP_loop_guided_start (start, end, incr, chunk_size, istart, iend);
#else   
  return _ompc_guided_sched_next((int*)istart, (int*)iend);
#endif    

}
bool XOMP_loop_runtime_start (long start, long end, long incr, long *istart, long *iend)
{
#ifdef USE_ROSE_GOMP_OPENMP_LIBRARY  
  return GOMP_loop_runtime_start (start, end, incr, istart, iend);
#else   
  return _ompc_runtime_sched_next((int*)istart, (int*)iend);
#endif    

}

 // loop ordered start
bool XOMP_loop_ordered_static_start (long start, long end, long incr, long chunk_size,long *istart, long *iend)
{
#ifdef USE_ROSE_GOMP_OPENMP_LIBRARY  
  return GOMP_loop_ordered_static_start (start, end, incr, chunk_size, istart, iend);
#else   
  return _ompc_static_sched_next ((int*)istart, (int*)iend); 
#endif    
}

bool XOMP_loop_ordered_dynamic_start (long start, long end, long incr, long chunk_size,long *istart, long *iend)
{
#ifdef USE_ROSE_GOMP_OPENMP_LIBRARY  
  return GOMP_loop_ordered_dynamic_start (start, end, incr, chunk_size, istart, iend);
#else   
  return _ompc_dynamic_sched_next ((int*)istart, (int*)iend); 
#endif    
}

bool XOMP_loop_ordered_guided_start (long start, long end, long incr, long chunk_size,long *istart, long *iend)
{
#ifdef USE_ROSE_GOMP_OPENMP_LIBRARY  
  return GOMP_loop_ordered_guided_start (start, end, incr, chunk_size, istart, iend);
#else   
  return _ompc_guided_sched_next ((int*)istart, (int*)iend); 
#endif    
}

bool XOMP_loop_ordered_runtime_start (long start, long end, long incr, long *istart, long *iend)
{
#ifdef USE_ROSE_GOMP_OPENMP_LIBRARY  
  return GOMP_loop_ordered_runtime_start (start, end, incr, istart, iend);
#else   
#endif    
}


// next
bool XOMP_loop_static_next (long * l, long *u)
{
#ifdef USE_ROSE_GOMP_OPENMP_LIBRARY  
  return GOMP_loop_static_next (l, u);
#else   
   return _ompc_static_sched_next((int*)l, (int*)u);
#endif    
}

bool XOMP_loop_dynamic_next (long *l, long *u)
{
#ifdef USE_ROSE_GOMP_OPENMP_LIBRARY  
  return GOMP_loop_dynamic_next (l, u);
#else   
   return _ompc_dynamic_sched_next((int*)l, (int*)u);
#endif    
}


bool XOMP_loop_guided_next (long *l, long *u)
{
#ifdef USE_ROSE_GOMP_OPENMP_LIBRARY  
  return GOMP_loop_guided_next (l, u);
#else   
   return _ompc_guided_sched_next((int *)l,(int *) u);
#endif    
}


bool XOMP_loop_runtime_next (long *l, long *u)
{
#ifdef USE_ROSE_GOMP_OPENMP_LIBRARY  
  return GOMP_loop_runtime_next (l, u);
#else   
   return _ompc_runtime_sched_next((int *)l, (int *)u);
#endif    
}


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

bool XOMP_loop_ordered_dynamic_next (long * a, long * b)
{
#ifdef USE_ROSE_GOMP_OPENMP_LIBRARY  
 return GOMP_loop_ordered_dynamic_next (a, b);
#else   
  return _ompc_dynamic_sched_next ((int *)a,(int *)b);
#endif    
}
bool XOMP_loop_ordered_guided_next (long *a, long *b)
{
#ifdef USE_ROSE_GOMP_OPENMP_LIBRARY  
 return GOMP_loop_ordered_guided_next (a, b);
#else   
  return _ompc_guided_sched_next ((int *)a,(int *)b);
#endif    
}
bool XOMP_loop_ordered_runtime_next (long *a, long *b)
{
#ifdef USE_ROSE_GOMP_OPENMP_LIBRARY  
 return GOMP_loop_ordered_runtime_next (a, b);
#else   
  return _ompc_runtime_sched_next ((int*)a,(int*)b);
#endif    
}

void XOMP_loop_end (void)
{
#ifdef USE_ROSE_GOMP_OPENMP_LIBRARY  
  GOMP_loop_end();
#else   
#endif    
}

void XOMP_loop_end_nowait (void)
{
#ifdef USE_ROSE_GOMP_OPENMP_LIBRARY  
  GOMP_loop_end_nowait();
#else   
#endif    
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

extern bool XOMP_single(void)
{
#ifdef USE_ROSE_GOMP_OPENMP_LIBRARY  
  return GOMP_single_start();
#else   
  return _ompc_do_single();
#endif    
}

extern bool XOMP_master(void)
{
#ifdef USE_ROSE_GOMP_OPENMP_LIBRARY  
  return (omp_get_thread_num() ==0);
#else   
  return _ompc_is_master ();
#endif    
}

void XOMP_atomic_start (void)
{
#ifdef USE_ROSE_GOMP_OPENMP_LIBRARY  
  GOMP_atomic_start();
#else   
  _ompc_atomic_lock();
#endif
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

