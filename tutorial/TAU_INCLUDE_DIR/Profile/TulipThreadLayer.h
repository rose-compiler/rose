/****************************************************************************
**			TAU Portable Profiling Package			   **
**			http://www.acl.lanl.gov/tau		           **
*****************************************************************************
**    Copyright 1997  						   	   **
**    Department of Computer and Information Science, University of Oregon **
**    Advanced Computing Laboratory, Los Alamos National Laboratory        **
****************************************************************************/
/***************************************************************************
**	File 		: TulipThreadLayer.h				  **
**	Description 	: TAU Profiling Package TulipThread Support Layer **
**	Author		: Sameer Shende					  **
**	Contact		: sameer@cs.uoregon.edu sameer@acl.lanl.gov 	  **
**	Flags		: Compile with				          **
**			  -DSMARTS for SMARTS interface instead of TULIP  **
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

#ifndef _TULIPTHREADLAYER_H_
#define _TULIPTHREADLAYER_H_

//////////////////////////////////////////////////////////////////////
//
// class TulipThreadLayer
//
// This class is used for supporting pthreads in RtsLayer class.
//////////////////////////////////////////////////////////////////////

#ifdef TULIPTHREADS
#ifdef SMARTS
#include <Mutex.h>
using Smarts::Mutex;
#else  // SMARTS
#include <Tulip_Mutex.h>
#endif // SMARTS
class TulipThreadLayer 
{ // Layer for RtsLayer to interact with pthreads 
  public:

 	TulipThreadLayer () { }  // defaults
	~TulipThreadLayer () { } 

	static int RegisterThread(void); // called before any profiling code
        static int InitializeThreadData(void);     // init thread mutexes
        static int InitializeDBMutexData(void);     // init tauDB mutex
        static int InitializeEnvMutexData(void);     // init tauEnv mutex
	static int GetThreadId(void); 	 // gets 0..N-1 thread id
	static int LockDB(void);	 // locks the tauDBMutex
	static int UnLockDB(void);	 // unlocks the tauDBMutex
	static int LockEnv(void);	 // locks the tauEnvMutex
	static int UnLockEnv(void);	 // unlocks the tauEnvMutex

  private:
#ifdef SMARTS
	static Mutex	   	tauDBMutex;  // to protect TheFunctionDB
	static Mutex	   	tauEnvMutex;  // second mutex
#else // SMARTS
	static Tulip_Mutex	   tauDBMutex;  // to protect TheFunctionDB
	static Tulip_Mutex	   tauEnvMutex;  // second mutex
#endif // SMARTS
	
};
#endif // TULIPTHREADS 

#endif // _TULIPTHREADLAYER_H_

	

/***************************************************************************
 * $RCSfile: TulipThreadLayer.h,v $   $Author: dquinlan $
 * $Revision: 1.1 $   $Date: 2006/04/24 00:22:01 $
 * POOMA_VERSION_ID: $Id: TulipThreadLayer.h,v 1.1 2006/04/24 00:22:01 dquinlan Exp $
 ***************************************************************************/


