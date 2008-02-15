/****************************************************************************
**			TAU Portable Profiling Package			   **
**			http://www.cs.uoregon.edu/research/paracomp/tau
*****************************************************************************
**    Copyright 2004  						   	   **
**    Department of Computer and Information Science, University of Oregon **
**    Advanced Computing Laboratory, Los Alamos National Laboratory        **
****************************************************************************/
/***************************************************************************
**	File 		: TauUtil.h					  **
**	Description 	: TAU Utilities 
**	Author		: Sameer Shende					  **
**	Contact		: sameer@cs.uoregon.edu 
**	Documentation	: See http://www.cs.uoregon.edu/research/paracomp/tau
***************************************************************************/

#ifndef _TAU_UTIL_H_
#define _TAU_UTIL_H_

#include <stdlib.h> 

/* The following macros help create a local array and assign to elements of 
   the local C array, values from Fortran array after conversion using f2c 
   MPI macros. Need to optimize the implementation. Use static array instead
   of malloc */
   

#if (defined(sgi)  || defined (TAU_WRAPPER_BLANK) || defined(__blrts__))
#define TAU_DECL_LOCAL(mtype, l) MPI_Fint * l
#define TAU_ALLOC_LOCAL(mtype, l, size) 
#define TAU_DECL_ALLOC_LOCAL(mtype, l, size) MPI_Fint * l
#define TAU_ASSIGN_VALUES(dest, src, size, func) dest = src 
#define TAU_ASSIGN_STATUS_F2C(dest, src, size, func) dest = src
#define TAU_ASSIGN_STATUS_C2F(dest, src, size, func) dest = src
#define TAU_FREE_LOCAL(l) 
#else
#define TAU_DECL_LOCAL(mtype, l) mtype *l
#define TAU_ALLOC_LOCAL(mtype, l, size) l = (mtype *) malloc(sizeof(mtype) * size)
#define TAU_DECL_ALLOC_LOCAL(mtype, l, size) TAU_DECL_LOCAL(mtype, l) = TAU_ALLOC_LOCAL(mtype, l, size) 
#define TAU_ASSIGN_VALUES(dest, src, size, func) { int i; for (i = 0; i < size; i++) dest[i] = func(src[i]); }

#define TAU_ASSIGN_STATUS_F2C(dest, src, size, func) { int i; for (i = 0; i < size; i++) func((MPI_Fint*)&((MPI_Status*)src)[i], &((MPI_Status*)dest)[i]); }
#define TAU_ASSIGN_STATUS_C2F(dest, src, size, func) { int i; for (i = 0; i < size; i++) func(&((MPI_Status*)src)[i], (MPI_Fint*)&((MPI_Status*)dest)[i]); }



#define TAU_FREE_LOCAL(l) free(l)
#endif /* sgi || TAU_MPI_NEEDS_STATUS */


/******************************************************/
#if (defined(sgi) || defined(TAU_MPI_NEEDS_STATUS))
#define MPI_Status_c2f(c,f) *(MPI_Status *)f=*(MPI_Status *)c 
#define MPI_Status_f2c(f,c) *(MPI_Status *)c=*(MPI_Status *)f
#endif /* sgi || TAU_MPI_NEEDS_STATUS */

#endif /* _TAU_UTIL_H_ */
