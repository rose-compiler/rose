/* General include file for all c routines in PGSLib */

/* $Id: pgslib-include-c.h,v 1.1 2004/07/07 10:27:17 dquinlan Exp $ */

#ifdef HAVE_CONFIG_H
/* #include <config.h> */
#endif

/*
#ifndef USE_MPI
#error "USE_MPI must be defined for compilation of PGSLIB"
#endif
*/
#define USE_MPI

/* #define USE_MPI */

#ifdef USE_MPI
#include "mpi.h"
#endif

/* Global defines used for all PGSLib C routines */
#define FALSE 0
#define TRUE  1

/* Need to know how F90 stores characters. This will be different on each system. */
#define BYTES_PER_CHAR 1

/* General information struct about state of the system */
#if 0
struct {
  int   initialized;
  int   nPE;
  int 	thisPE;
  int   io_pe;
} pgslib_state;
#else
/* Quinlan (9/24/2000) Added here to avoid redeclaration of pgslib_state (prevents warning on SGI at link time) */
struct pgslib_state_struct {
  int   initialized;
  int   nPE;
  int 	thisPE;
  int   io_pe;
};

extern struct pgslib_state_struct pgslib_state;
#endif

#ifndef INCLUDE_NAME_TRANSLATOR
#include "pgslib-names.h"
#endif
