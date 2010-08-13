/****************************************************************************
**			TAU Portable Profiling Package			   **
**			http://www.acl.lanl.gov/tau		           **
*****************************************************************************
**    Copyright 1997  						   	   **
**    Department of Computer and Information Science, University of Oregon **
**    Advanced Computing Laboratory, Los Alamos National Laboratory        **
****************************************************************************/
/***************************************************************************
**	File 		: Profiler.h					  **
**	Description 	: TAU Profiling Package				  **
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
#ifndef PROFILER_H
#define PROFILER_H

#if (defined(TAU_WINDOWS))
#pragma warning( disable : 4786 )
#endif /* TAU_WINDOWS */

#if (defined (__cplusplus ) && !defined (TAU_USE_C_API))

#include <Profile/ProfileGroups.h>

#ifdef TAU_MULTIPLE_COUNTERS
#define MAX_TAU_COUNTERS 25
#endif//TAU_MULTIPLE_COUNTERS

#if (defined(PTHREADS) || defined(TULIPTHREADS) || defined(JAVA) || defined(TAU_WINDOWS) || defined (TAU_OPENMP) || defined (TAU_SPROC))


#ifndef TAU_MAX_THREADS

#ifdef TAU_CHARM
#define TAU_MAX_THREADS 512
#else
#define TAU_MAX_THREADS 128
#endif

#endif

#else
#define TAU_MAX_THREADS 1
#endif /* PTHREADS || TULIPTHREADS || JAVA || TAU_WINDOWS || OPENMP || SPROC */

#include <Profile/TauAPI.h>

#if (defined(PROFILING_ON) || defined(TRACING_ON))

#include <Profile/ProfileHeaders.h>

#include <Profile/PthreadLayer.h>

#include <Profile/TulipThreadLayer.h>

#include <Profile/JavaThreadLayer.h>

#include <Profile/SprocLayer.h>

#include <Profile/RtsLayer.h>

#include <Profile/FunctionInfo.h>

#include <Profile/UserEvent.h>

#include <Profile/PclLayer.h>

#include <Profile/PapiLayer.h>

#include <Profile/MultipleCounters.h>

#include <Profile/WindowsThreadLayer.h>

#include <Profile/TauMuse.h>

#include <Profile/TauCompensate.h>

#include <Profile/TauHandler.h>

//////////////////////////////////////////////////////////////////////
//
// class Profiler
//
// This class is intended to be instantiated once per function
// (or other code block to be timed) as an auto variable.
//
// It will be constructed each time the block is entered
// and destroyed when the block is exited.  The constructor
// turns on the timer, and the destructor turns it off.
//
//////////////////////////////////////////////////////////////////////

// #error "Compiling TAU Profiler class"

class Profiler
{
public:
	Profiler(FunctionInfo * fi, TauGroup_t ProfileGroup = TAU_DEFAULT, 
                 bool StartStop = false, int tid = RtsLayer::myThread());

	void Start(int tid = RtsLayer::myThread());
	Profiler(const Profiler& X);
	Profiler& operator= (const Profiler& X);
	// Clean up data from this invocation.
	void Stop(int tid = RtsLayer::myThread());
	~Profiler();
#ifdef TAU_CALLPATH
        void CallPathStart(int tid);
#ifdef TAU_MULTIPLE_COUNTERS
        void CallPathStop(double* totaltime, int tid);
#else  // TAU_MULTIPLE_COUNTERS 
        void CallPathStop(double totaltime, int tid);
#endif // TAU_MULTIPLE_COUNTERS 
#endif // TAU_CALLPATH 
  	static void ProfileExit(const char *message=0, 
                                int tid = RtsLayer::myThread());
        static bool createDirectories();
	static int StoreData(int tid = RtsLayer::myThread()); 
	static int DumpData(bool increment = false,
                            int tid = RtsLayer::myThread(), char *prefix = "dump"); 
        static void PurgeData(int tid = RtsLayer::myThread());
	static void theFunctionList(const char ***inPtr, int *numOfFunctions,
                                    bool addName = false, const char * inString = NULL);
        static void dumpFunctionNames();
#ifndef TAU_MULTIPLE_COUNTERS
	static void theCounterList(const char ***inPtr, int *numOfCounters);
#endif //TAU_MULTIPLE_COUNTERS

	static void getFunctionValues(const char **inFuncs,
                                      int numOfFuncs,
				      double ***counterExclusiveValues,
				      double ***counterInclusiveValues,
				      int **numOfCalls,
				      int **numOfSubRoutines,
				      const char ***counterNames,
				      int *numOfCounters,
				      int tid = RtsLayer::myThread());
        static int dumpFunctionValues(const char **inFuncs,
                                      int numOfFuncs,
				      bool increment = false,
				      int tid = RtsLayer::myThread(), 
				      char *prefix = "dump");

	static void getUserEventList(const char ***inPtr, int *numUserEvents);

	static void getUserEventValues(const char **inUserEvents, int numUserEvents,
                                       int **numEvents, double **max, double **min,
				       double **mean, double **sumSqr, 
				       int tid = RtsLayer::myThread());

#if ( defined(PROFILE_CALLS) || defined(PROFILE_STATS) || defined(PROFILE_CALLSTACK) ) 
	int ExcludeTimeThisCall(double t);
	double ExclTimeThisCall; // for this invocation of the function
#endif /* PROFILE_CALLS || PROFILE_STATS */

	static Profiler * CurrentProfiler[TAU_MAX_THREADS];
#ifndef TAU_MULTIPLE_COUNTERS
	double StartTime;
#else //TAU_MULTIPLE_COUNTERS
	double StartTime[MAX_TAU_COUNTERS];
	double CurrentTime[MAX_TAU_COUNTERS];
	double TotalTime[MAX_TAU_COUNTERS];
#endif//TAU_MULTIPLE_COUNTERS
	FunctionInfo * ThisFunction;
#ifdef TAU_CALLPATH
	FunctionInfo * CallPathFunction;
	bool 	       AddInclCallPathFlag; 
#endif // TAU_CALLPATH
#ifdef TAU_COMPENSATE
	/* Compensate for instrumentation overhead based on total number of 
	child calls executed under the given timer */
	long NumChildren;
	void SetNumChildren(long n);
	long GetNumChildren(void);
	void AddNumChildren(long value);
#endif /* TAU_COMPENSATE */
	Profiler * ParentProfiler; 



#ifdef PROFILE_CALLSTACK
  	double InclTime_cs;
  	double ExclTime_cs;
  	static void CallStackTrace(int tid = RtsLayer::myThread());
#endif /* PROFILE_CALLSTACK  */
#ifdef TAU_PROFILEPHASE
	bool GetPhase(void);
	void SetPhase(bool flag);
#endif /* TAU_PROFILEPHASE */
#ifdef TAU_DEPTH_LIMIT
	int  GetDepthLimit(void);
	void SetDepthLimit(int value);
#endif /* TAU_DEPTH_LIMIT */ 
#ifdef TAU_MPITRACE
	void EnableAllEventsOnCallStack(int tid, Profiler *current);
#endif /* TAU_MPITRACE */

private:
	TauGroup_t MyProfileGroup_;
	bool	StartStopUsed_;
	bool 	AddInclFlag; 
	/* There is a class that will do some initialization
           of FunctionStack that can't be done with
	   just the constructor.
	   friend class ProfilerInitializer; */
        bool    PhaseFlag;
#ifdef TAU_DEPTH_LIMIT
	int  profiledepth; 
#endif /* TAU_DEPTH_LIMIT */

#ifdef TAU_MPITRACE
	bool 	RecordEvent; /* true when an MPI call is in the callpath */
#endif /* TAU_MPITRACE */
};


#endif /* PROFILING_ON || TRACING_ON */
#include <Profile/TauMapping.h>
// included after class Profiler is defined.
#else /* __cplusplus && ! TAU_USE_C_API */
#include <Profile/TauCAPI.h> /* For C program */
#endif /* __cplusplus && ! TAU_USE_C_API */

#endif /* PROFILER_H */
/***************************************************************************
 * $RCSfile: Profiler.h,v $   $Author: dquinlan $
 * $Revision: 1.1 $   $Date: 2006/04/24 00:22:00 $
 * POOMA_VERSION_ID: $Id: Profiler.h,v 1.1 2006/04/24 00:22:00 dquinlan Exp $ 
 ***************************************************************************/
