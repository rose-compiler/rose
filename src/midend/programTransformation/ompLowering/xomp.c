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

