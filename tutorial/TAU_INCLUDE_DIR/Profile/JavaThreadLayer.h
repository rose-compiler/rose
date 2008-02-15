/****************************************************************************
**			TAU Portable Profiling Package			   **
**			http://www.acl.lanl.gov/tau		           **
*****************************************************************************
**    Copyright 1997  						   	   **
**    Department of Computer and Information Science, University of Oregon **
**    Advanced Computing Laboratory, Los Alamos National Laboratory        **
****************************************************************************/
/***************************************************************************
**	File 		: JavaThreadLayer.h				  **
**	Description 	: TAU Profiling Package Java Thread Support Layer **
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

#ifndef _JAVATHREADLAYER_H_
#define _JAVATHREADLAYER_H_

//////////////////////////////////////////////////////////////////////
//
// class PthreadLayer
//
// This class is used for supporting pthreads in RtsLayer class.
//////////////////////////////////////////////////////////////////////

#ifdef JAVA
#include <jvmpi.h>
class  JavaThreadLayer
{ // Layer for JavaThreadLayer to interact with Java Threads 
  public:
 	
 	JavaThreadLayer () { }  // defaults
	~JavaThreadLayer () { } 

	static int * RegisterThread(JNIEnv * env_id); 
        static int InitializeThreadData(void);     // init thread mutexes
        static int InitializeDBMutexData(void);     // init tauDB mutex
        static int InitializeEnvMutexData(void);     // init tauEnv mutex
	static int GetThreadId(void); 	 	 // gets 0..N-1 thread id
	static int GetThreadId(JNIEnv *env_id);	 // gets 0..N-1 thread id
	static int LockDB(void);	 // locks the tauDBMutex
	static int UnLockDB(void);	 // unlocks the tauDBMutex
	static int LockEnv(void);	 // locks the tauEnvMutex
	static int UnLockEnv(void);	 // unlocks the tauEnvMutex
	static int TotalThreads(void); 	 // returns the thread count
        // return the current thread's cpu time, in nanoseconds (as reported by jvmpi)
        static jlong getCurrentThreadCpuTime(void); 

	static JVMPI_Interface 	    *tau_jvmpi_interface;
	static JavaVM 	   	    *tauVM; 	     // Virtual machine 
  private:
        static int		    tauThreadCount;  // Number of threads
	static JVMPI_RawMonitor     tauNumThreadsLock; // to protect counter
	static JVMPI_RawMonitor     tauDBMutex; // to protect counter
	static JVMPI_RawMonitor     tauEnvMutex; // second mutex
};
#endif // JAVA 

#endif // _JAVATHREADLAYER_H_

	

/***************************************************************************
 * $RCSfile: JavaThreadLayer.h,v $   $Author: dquinlan $
 * $Revision: 1.1 $   $Date: 2006/04/24 00:22:00 $
 * POOMA_VERSION_ID: $Id: JavaThreadLayer.h,v 1.1 2006/04/24 00:22:00 dquinlan Exp $
 ***************************************************************************/


