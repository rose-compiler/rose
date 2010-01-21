#include "rose_config.h"
#include "libxomp.h"

#ifdef GCC_GOMP_OPENMP_LIB_PATH 
// GOMP header
#include "libgomp_g.h"
#else
// Omni header
#include "libompc.h"
#endif

// avoid include omp.h 
extern bool omp_get_thread_num();

#include <stdlib.h> // for getenv()
#include <stdio.h> // for getenv()
#include <assert.h>

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


//Runtime library initialization routine
void XOMP_init (int argc, char ** argv)
{
#ifdef GCC_GOMP_OPENMP_LIB_PATH  
#else   
  _ompc_init (argc, argv);
#endif    
}

// Runtime library termination routine
void XOMP_terminate (int exitcode)
{
#ifdef GCC_GOMP_OPENMP_LIB_PATH  
#else   
  _ompc_terminate (exitcode);
#endif    
}

void XOMP_parallel_start (void (*func) (void *), void *data, unsigned numThread)
{

#ifdef GCC_GOMP_OPENMP_LIB_PATH  
  GOMP_parallel_start (func, data, numThread);
  func(data);
#else   
  _ompc_do_parallel (func, data); 
#endif    
}

void XOMP_parallel_end (void)
{
#ifdef GCC_GOMP_OPENMP_LIB_PATH  
  GOMP_parallel_end ();
#else   
#endif    
}

void XOMP_task (void (*fn) (void *), void *data, void (*cpyfn) (void *, void *),
                       long arg_size, long arg_align, bool if_clause, unsigned untied)
{

#ifdef GCC_GOMP_OPENMP_LIB_PATH  
// only gcc 4.4.x has task support
#if __GNUC__ > 4 || \
  (__GNUC__ == 4 && (__GNUC_MINOR__ > 4 || \
                     (__GNUC_MINOR__ == 4 && \
                      __GNUC_PATCHLEVEL__ >= 0)))

 GOMP_task (fn, data, cpyfn, arg_size, arg_align, if_clause, untied);
#endif

#else
#endif 
}
void XOMP_taskwait (void)
{
#ifdef GCC_GOMP_OPENMP_LIB_PATH  
#if __GNUC__ > 4 || \
  (__GNUC__ == 4 && (__GNUC_MINOR__ > 4 || \
                     (__GNUC_MINOR__ == 4 && \
                      __GNUC_PATCHLEVEL__ >= 0)))

  GOMP_taskwait();
#endif

#else
#endif 
}

// scheduler initialization, only meaningful used for OMNI
void XOMP_loop_static_init(int lower, int upper, int stride, int chunk_size)
{
#ifdef GCC_GOMP_OPENMP_LIB_PATH  
  // empty operation for gomp
#else   
  _ompc_static_sched_init (lower, upper, stride, chunk_size);
#endif    
}

void XOMP_loop_dynamic_init(int lower, int upper, int stride, int chunk_size)
{
#ifdef GCC_GOMP_OPENMP_LIB_PATH  
#else   
  _ompc_dynamic_sched_init (lower, upper, stride, chunk_size);
#endif    
}
void XOMP_loop_guided_init(int lower, int upper, int stride, int chunk_size)
{

#ifdef GCC_GOMP_OPENMP_LIB_PATH  
#else   
  _ompc_guided_sched_init (lower, upper, stride, chunk_size);
#endif    
}

void XOMP_loop_runtime_init(int lower, int upper, int stride)
{
#ifdef GCC_GOMP_OPENMP_LIB_PATH  
#else   
  _ompc_runtime_sched_init (lower, upper, stride);
#endif    
}

// if (start), 
bool XOMP_loop_static_start (long start, long end, long incr, long chunk_size,long *istart, long *iend)
{
#ifdef GCC_GOMP_OPENMP_LIB_PATH  
  return GOMP_loop_static_start (start, end, incr, chunk_size, istart, iend);
#else   
  return _ompc_static_sched_next(istart, iend);
#endif    
}

bool XOMP_loop_dynamic_start (long start, long end, long incr, long chunk_size,long *istart, long *iend)
{
#ifdef GCC_GOMP_OPENMP_LIB_PATH  
  return GOMP_loop_dynamic_start (start, end, incr, chunk_size, istart, iend);
#else   
  return _ompc_dynamic_sched_next(istart, iend);
#endif    

}
bool XOMP_loop_guided_start (long start, long end, long incr, long chunk_size,long *istart, long *iend)
{
#ifdef GCC_GOMP_OPENMP_LIB_PATH  
  return GOMP_loop_guided_start (start, end, incr, chunk_size, istart, iend);
#else   
  return _ompc_guided_sched_next(istart, iend);
#endif    

}
bool XOMP_loop_runtime_start (long start, long end, long incr, long *istart, long *iend)
{
#ifdef GCC_GOMP_OPENMP_LIB_PATH  
  return GOMP_loop_runtime_start (start, end, incr, istart, iend);
#else   
  return _ompc_runtime_sched_next(istart, iend);
#endif    

}

 // loop ordered start
bool XOMP_loop_ordered_static_start (long start, long end, long incr, long chunk_size,long *istart, long *iend)
{
#ifdef GCC_GOMP_OPENMP_LIB_PATH  
  return GOMP_loop_ordered_static_start (start, end, incr, chunk_size, istart, iend);
#else   
#endif    
}

bool XOMP_loop_ordered_dynamic_start (long start, long end, long incr, long chunk_size,long *istart, long *iend)
{
#ifdef GCC_GOMP_OPENMP_LIB_PATH  
  return GOMP_loop_ordered_dynamic_start (start, end, incr, chunk_size, istart, iend);
#else   
#endif    
}

bool XOMP_loop_ordered_guided_start (long start, long end, long incr, long chunk_size,long *istart, long *iend)
{
#ifdef GCC_GOMP_OPENMP_LIB_PATH  
  return GOMP_loop_ordered_guided_start (start, end, incr, chunk_size, istart, iend);
#else   
#endif    
}

bool XOMP_loop_ordered_runtime_start (long start, long end, long incr, long *istart, long *iend)
{
#ifdef GCC_GOMP_OPENMP_LIB_PATH  
  return GOMP_loop_ordered_runtime_start (start, end, incr, istart, iend);
#else   
#endif    
}


// next
bool XOMP_loop_static_next (long * l, long *u)
{
#ifdef GCC_GOMP_OPENMP_LIB_PATH  
  return GOMP_loop_static_next (l, u);
#else   
   return _ompc_static_sched_next(l, u);
#endif    
}

bool XOMP_loop_dynamic_next (long *l, long *u)
{
#ifdef GCC_GOMP_OPENMP_LIB_PATH  
  return GOMP_loop_dynamic_next (l, u);
#else   
   return _ompc_dynamic_sched_next(l, u);
#endif    
}


bool XOMP_loop_guided_next (long *l, long *u)
{
#ifdef GCC_GOMP_OPENMP_LIB_PATH  
  return GOMP_loop_guided_next (l, u);
#else   
   return _ompc_guided_sched_next(l, u);
#endif    
}


bool XOMP_loop_runtime_next (long *l, long *u)
{
#ifdef GCC_GOMP_OPENMP_LIB_PATH  
  return GOMP_loop_runtime_next (l, u);
#else   
   return _ompc_runtime_sched_next(l, u);
#endif    
}


bool XOMP_loop_ordered_static_next (long *a, long * b)
{
#ifdef GCC_GOMP_OPENMP_LIB_PATH  
  return XOMP_loop_ordered_static_next (a, b);
#else   
#endif    
}

bool XOMP_loop_ordered_dynamic_next (long * a, long * b)
{
#ifdef GCC_GOMP_OPENMP_LIB_PATH  
 return XOMP_loop_ordered_dynamic_next (a, b);
#else   
#endif    
}
bool XOMP_loop_ordered_guided_next (long *a, long *b)
{
#ifdef GCC_GOMP_OPENMP_LIB_PATH  
 return XOMP_loop_ordered_guided_next (a, b);
#else   
#endif    
}
bool XOMP_loop_ordered_runtime_next (long *a, long *b)
{
#ifdef GCC_GOMP_OPENMP_LIB_PATH  
 return XOMP_loop_ordered_runtime_next (a, b);
#else   
#endif    
}

void XOMP_loop_end (void)
{
#ifdef GCC_GOMP_OPENMP_LIB_PATH  
  GOMP_loop_end();
#else   
#endif    
}

void XOMP_loop_end_nowait (void)
{
#ifdef GCC_GOMP_OPENMP_LIB_PATH  
  GOMP_loop_end_nowait();
#else   
#endif    
}

void XOMP_barrier (void)
{
#ifdef GCC_GOMP_OPENMP_LIB_PATH  
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
#ifdef GCC_GOMP_OPENMP_LIB_PATH  
    GOMP_critical_name_start(data);
#else   
    _ompc_enter_critical(data);
#endif    
}

void XOMP_critical_end (void** data)
{
#ifdef GCC_GOMP_OPENMP_LIB_PATH  
    GOMP_critical_name_end(data);
#else   
    _ompc_exit_critical(data);
#endif    
}

extern bool XOMP_single(void)
{
#ifdef GCC_GOMP_OPENMP_LIB_PATH  
  GOMP_single_start();
#else   
  _ompc_do_single();
#endif    
}

extern bool XOMP_master(void)
{
#ifdef GCC_GOMP_OPENMP_LIB_PATH  
  return (omp_get_thread_num() ==0);
#else   
  return _ompc_is_master ();
#endif    
}

void XOMP_atomic_start (void)
{
#ifdef GCC_GOMP_OPENMP_LIB_PATH  
  GOMP_atomic_start();
#else   
  _ompc_atomic_lock();
#endif
}

void XOMP_atomic_end (void)
{

#ifdef GCC_GOMP_OPENMP_LIB_PATH  
   GOMP_atomic_end();
#else   
  _ompc_atomic_unlock();
#endif
}

void XOMP_flush_all ()
{
#ifdef GCC_GOMP_OPENMP_LIB_PATH  
  __sync_synchronize();
#else   
  _ompc_flush(0,0);
#endif
}

void XOMP_flush_one(char * startAddress, int nbyte)
{
#ifdef GCC_GOMP_OPENMP_LIB_PATH  
  __sync_synchronize();
#else   
  _ompc_flush(startAddress,nbyte);
#endif
}

void XOMP_ordered_start (void)
{
#ifdef GCC_GOMP_OPENMP_LIB_PATH  
   GOMP_ordered_start();
#else   
#endif

}
void XOMP_ordered_end (void)
{
#ifdef GCC_GOMP_OPENMP_LIB_PATH  
  GOMP_ordered_end();
#else   

#endif

}

