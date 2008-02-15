/****************************************************************************
**			TAU Portable Profiling Package			   **
**			http://www.acl.lanl.gov/tau		           **
*****************************************************************************
**    Copyright 1997  						   	   **
**    Department of Computer and Information Science, University of Oregon **
**    Advanced Computing Laboratory, Los Alamos National Laboratory        **
****************************************************************************/
/***************************************************************************
**	File 		: OpenMPLayer.h  				  **
**	Description 	: TAU Profiling Package OpenMP Support Layer	  **
**	Author		: Sameer Shende					  **
**	Contact		: sameer@cs.uoregon.edu sameer@acl.lanl.gov 	  **
**	Flags		: Compile with				          **
**			  -DPROFILING_ON to enable profiling (ESSENTIAL)  **
**			  -DPROFILE_STATS for Std. Deviation of Excl Time **
**			  -DSGI_HW_COUNTERS for using SGI counters 	  **
**			  -DPROFILE_CALLS  for trace of each invocation   **
**			  -DSGI_TIMERS  for SGI fast nanosecs timer	  **
**			  -DTULIP_TIMERS for non-sgi Platform	 	  **
**			  -DPOOMA_STDSTL for using STD STL in POOMA src   **
**			  -DPOOMA_TFLOP for Intel Teraflop at SNL/NM 	  **
**			  -DPOOMA_KAI for KCC compiler 			  **
**			  -DDEBUG_PROF  for internal debugging messages   **
**                        -DPROFILE_CALLSTACK to enable callstack traces  **
**	Documentation	: See http://www.acl.lanl.gov/tau	          **
***************************************************************************/

#ifndef _TAU_OPENMP_LAYER_H_
#define _TAU_OPENMP_LAYER_H_

//////////////////////////////////////////////////////////////////////
//
// class OpenMPLayer
//
// This class is used for supporting OpenMP Threads in RtsLayer class.
//////////////////////////////////////////////////////////////////////

#ifdef TAU_OPENMP
extern "C" {
#include <omp.h>
#ifndef _OPENMP
#define _OPENMP
#endif /* _OPENMP */
}
class OpenMPLayer 
{ // Layer for RtsLayer to interact with OpenMP
  public:
 	
 	OpenMPLayer () { }  // defaults
	~OpenMPLayer () { } 

	static int RegisterThread(void); // called before any profiling code
        static int InitializeThreadData(void);     // init thread mutexes
        static int InitializeDBMutexData(void);     // init tauDB mutex
        static int InitializeEnvMutexData(void);     // init tauEnv mutex
	static int GetThreadId(void); 	 // gets 0..N-1 thread id
	static int TotalThreads(void);   // gets number of threads
	static int LockDB(void);	 // locks the tauDBMutex
	static int UnLockDB(void);	 // unlocks the tauDBMutex
	static int LockEnv(void);	 // locks the tauEnvMutex
	static int UnLockEnv(void);	 // unlocks the tauEnvMutex

  private:
	static omp_lock_t tauDBmutex;  // to protect TheFunctionDB
	static omp_lock_t tauEnvmutex;  // second lock
	
};
#endif // TAU_OPENMP

#endif // _TAU_OPENMP_LAYER_H_

	

/***************************************************************************
 * $RCSfile: OpenMPLayer.h,v $   $Author: dquinlan $
 * $Revision: 1.1 $   $Date: 2006/04/24 00:22:00 $
 * POOMA_VERSION_ID: $Id: OpenMPLayer.h,v 1.1 2006/04/24 00:22:00 dquinlan Exp $
 ***************************************************************************/


