/* This file contains any configuration-dependent data for mpi.h */
#ifndef MPI_DEFS
#define MPI_DEFS

/* Include Fortran definitions */
#if 0 == 0
#include "mpi_fortdefs.h"
#else
typedef int MPI_Fint;
#endif
typedef int MPI_Aint;

/* MPI_STATUS_SIZE is not strictly required in C; however, it should match
   the value for Fortran */
#define MPI_STATUS_SIZE 4

/* 
   Status object.  It is the only user-visible MPI data-structure 
   The "count" field is PRIVATE; use MPI_Get_count to access it. 
 */
typedef struct { 
    int count;
    int MPI_SOURCE;
    int MPI_TAG;
    int MPI_ERROR;
#if (MPI_STATUS_SIZE > 4)
    int extra[MPI_STATUS_SIZE - 4];
#endif
} MPI_Status;

/* Eventually, these should be integrated into mpi.h by using an mpi.h.in 
   file */
/* If const is not supported, define HAVE_NO_C_CONST here */
#if 0 && !defined(HAVE_NO_C_CONST)
#define HAVE_NO_C_CONST 1
#endif

/* If stdargs supported, define USE_STDARG here */
#if 1 && !defined(USE_STDARG)
#define USE_STDARG 1
#endif

#if 0
#  include "mpid_defs.h"
#endif

#define ROMIO 1
#if ROMIO == 1
#include "mpio.h"
#endif

#endif
