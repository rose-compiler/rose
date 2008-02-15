/****************************************************************************
**			TAU Portable Profiling Package			   **
**			http://www.cs.uoregon.edu/research/paracomp/tau    **
*****************************************************************************
**    Copyright 2005  						   	   **
**    Department of Computer and Information Science, University of Oregon **
**    Advanced Computing Laboratory, Los Alamos National Laboratory        **
**    Research Center Juelich, Germany                                     **
****************************************************************************/
/****************************************************************************
**	File 		: tau_types.h					   **
**	Description 	: An attempt to consolidate type size (primarily   **
**                        for traces)                                      **
**	Author		: Alan Morris					   **
**	Contact		: amorris@cs.uoregon.edu 	                   **
****************************************************************************/



#ifdef TAU_WINDOWS
typedef char x_uint8;
typedef short x_uint16;
typedef int x_uint32;
typedef __int64 x_uint64;

typedef unsigned char x_uint8;
typedef unsigned short x_uint16;
typedef unsigned int32 x_uint32;
typedef unsigned __int64 x_uint64;
#else
typedef char x_int8;
typedef short x_int16;
typedef int x_int32;
typedef long long x_int64;

typedef unsigned char x_uint8;
typedef unsigned short x_uint16;
typedef unsigned int x_uint32;
typedef unsigned long long x_uint64;
#endif
