/*********************************************************************/
/*                  pC++/Sage++  Copyright (C) 1994                  */
/*  Indiana University  University of Oregon  University of Rennes   */
/*********************************************************************/

/*
 * pcxx_machines.h: machine specific definitions 
 * 
 * Bernd Mohr
 *
 */

/* Support for _SMRTS_ added (Pete Beckman) for shared memory */
/* machines.  _SMRTS_ is undefined at the end if UNIPROC or __PVM__, */
/* or __MPI__ is defined */

# ifndef __PCXX_MACHINES_H__
# define __PCXX_MACHINES_H__

# if defined (butterfly)
#   define PCXX_MAXPROCS 256
#   define PCXX_MALLOC malloc
#   define PCXX_SUGGESTED_MSG_SIZE -1
#   define _SMRTS_
# else
# if defined (__ksr__)
#   define PCXX_MAXPROCS 1088
#   define PCXX_MALLOC malloc
#   define PCXX_SUGGESTED_MSG_SIZE -1
#   define _SMRTS_
# else
# if defined(__SGIMP__)
#   define PCXX_MAXPROCS 4096
#   define PCXX_MALLOC malloc
#   define PCXX_SUGGESTED_MSG_SIZE -1
#   define _SMRTS_
# else
# if defined(_SEQUENT_) || defined(sequent)
#   define PCXX_MAXPROCS 30
#   define PCXX_MALLOC shmalloc
#   define PCXX_SUGGESTED_MSG_SIZE -1
#   define _SMRTS_
#   undef SIGSEGV
#   ifdef __cplusplus
      extern "C" char *shmalloc();
#   else
      extern char *shmalloc();
#   endif
# else
# if defined(__PARAGON__)
#   define PCXX_MAXPROCS 1024
#   define PCXX_MALLOC malloc
#   define PCXX_SUGGESTED_MSG_SIZE 16384
# else
# if defined(__SP1__)
#   define PCXX_MAXPROCS 256
#   define PCXX_MALLOC malloc
#   define PCXX_SUGGESTED_MSG_SIZE -1
# else
# if defined(CM5)
#   define PCXX_MAXPROCS 1024
#   define PCXX_MALLOC malloc
#   define PCXX_SUGGESTED_MSG_SIZE 16384
# else
# if defined(__MEIKO__)
#   define PCXX_MAXPROCS 64
#   define PCXX_MALLOC malloc
#   define PCXX_SUGGESTED_MSG_SIZE 16384
# else
# if defined(__PVM__)
#   define PCXX_MAXPROCS 256
#   define PCXX_MALLOC malloc
#   define PCXX_SUGGESTED_MSG_SIZE 8192
# else
# if defined(__T3D__)
#   define PCXX_MAXPROCS 256
#   define PCXX_MALLOC malloc
#   define PCXX_SUGGESTED_MSG_SIZE -1
# else
# if defined(_CRAYT3E)
#   define PCXX_MAXPROCS 512
#   define PCXX_MALLOC malloc
#   define PCXX_SUGGESTED_MSG_SIZE -1
# else
#   define PCXX_MAXPROCS 4096
#   define PCXX_MALLOC malloc
#   define PCXX_SUGGESTED_MSG_SIZE -1
# endif
# endif
# endif
# endif
# endif
# endif
# endif
# endif
# endif
# endif
# endif

# ifndef PCXX_MYNODE
#   ifdef UNIPROC
#     define PCXX_MYNODE   0
#   else
#     ifndef __ksr__
#       define __private
#     endif
#     define PCXX_MYNODE   RtsLayer::myNode()
      __private extern int MyNodeNumber;
#   endif
#   define PCXX_MYTHREAD 0
# endif

# ifdef UNIPROC
#  ifdef _SMRTS_
#    undef _SMRTS_
#  endif
# endif

# ifdef __PVM__
#  ifdef _SMRTS_
#    undef _SMRTS_
#  endif
# endif

# ifdef __MPI__
#  ifdef _SMRTS_
#    undef _SMRTS_
#  endif
# endif


# endif
