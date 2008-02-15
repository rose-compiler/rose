/****************************************************************************
**			TAU Portable Profiling Package			   **
**			http://www.acl.lanl.gov/tau		           **
*****************************************************************************
**    Copyright 1997  						   	   **
**    Department of Computer and Information Science, University of Oregon **
**    Advanced Computing Laboratory, Los Alamos National Laboratory        **
****************************************************************************/
/***************************************************************************
**	File 		: WindowsThreadLayer.h				  **
**	Description 	: TAU Profiling Package Microsoft Windows Thread Support Layer	**
**	Author		: Robert Ansell-Bell					  **
**	Contact		: bertie@cs.uoregon.edu 	  **
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

#ifndef _WINDOWSTHREADLAYER_H_
#define _WINDOWSTHREADLAYER_H_

//////////////////////////////////////////////////////////////////////
//
// class WindowsThreadLayer
//
// This class is used for supporting Microsoft Windows' threads in RtsLayer class.
//////////////////////////////////////////////////////////////////////


#ifdef TAU_WINDOWS

//////////////////////////////////////////////////////////////////////
// Include Files 
//////////////////////////////////////////////////////////////////////
#include <windows.h>


class WindowsThreadLayer 
{ // Layer for RtsLayer to interact with Microsoft Windows' threads. 
  public:
 	
 	WindowsThreadLayer () { }  // defaults
	~WindowsThreadLayer () { } 

	static int RegisterThread(void); // called before any profiling code
        static int InitializeThreadData(void);     // init thread mutexes
        static int InitializeDBMutexData(void);     // init tauDB mutex
        static int InitializeEnvMutexData(void);     // init tauDB mutex
	static int GetThreadId(void); 	 // gets 0..N-1 thread id
	static int LockDB(void);	 // locks the tauDBMutex
	static int UnLockDB(void);	 // unlocks the tauDBMutex
	static int LockEnv(void);	 // locks the tauEnvMutex
	static int UnLockEnv(void);	 // unlocks the tauEnvMutex

  private:
	static DWORD		 	   tauWindowsthreadId; // tid 
	static HANDLE			   tauThreadcountMutex; // to protect counter 
	static int 				   tauThreadCount;     // counter
	static HANDLE			   tauDBMutex;  // to protect TheFunctionDB
	static HANDLE			   tauEnvMutex;  // to protect TheFunctionDB
	
};

#endif //TAU_WINDOWS

#endif // _WINDOWSTHREADLAYER_H_
