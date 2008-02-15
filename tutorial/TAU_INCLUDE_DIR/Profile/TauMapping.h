/****************************************************************************
**			TAU Portable Profiling Package			   **
**			http://www.acl.lanl.gov/tau		           **
*****************************************************************************
**    Copyright 1999  						   	   **
**    Department of Computer and Information Science, University of Oregon **
**    Advanced Computing Laboratory, Los Alamos National Laboratory        **
****************************************************************************/
/***************************************************************************
**	File 		: Profiler.cpp					  **
**	Description 	: TAU Mappings for relating profile data from one **
**			  layer to another				  **
**	Author		: Sameer Shende					  **
**	Contact		: sameer@cs.uoregon.edu sameer@acl.lanl.gov 	  **
**	Flags		: Compile with				          **
**			  -DPROFILING_ON to enable profiling (ESSENTIAL)  **
**			  -DPROFILE_STATS for Std. Deviation of Excl Time **
**			  -DSGI_HW_COUNTERS for using SGI counters 	  **
**			  -DPROFILE_CALLS  for trace of each invocation   **
**                        -DSGI_TIMERS  for SGI fast nanosecs timer       **
**			  -DTULIP_TIMERS for non-sgi Platform	 	  **
**			  -DPOOMA_STDSTL for using STD STL in POOMA src   **
**			  -DPOOMA_TFLOP for Intel Teraflop at SNL/NM 	  **
**			  -DPOOMA_KAI for KCC compiler 			  **
**			  -DDEBUG_PROF  for internal debugging messages   **
**                        -DPROFILE_CALLSTACK to enable callstack traces  **
**	Documentation	: See http://www.acl.lanl.gov/tau	          **
***************************************************************************/
/* TAU Mappings */
#ifndef _TAU_MAPPING_H_
#define _TAU_MAPPING_H_

#if (PROFILING_ON || TRACING_ON)
// For Mapping, global variables used between layers
FunctionInfo *& TheTauMapFI(TauGroup_t ProfileGroup=TAU_DEFAULT);


#if (TAU_MAX_THREADS == 1)
// If we're not multi-threaded, just use the non-thread-safe static initializer

#define TAU_MAPPING(stmt, group)   \
  { \
    static FunctionInfo TauMapFI(#stmt, " " , group, #group); \
    static Profiler *TauMapProf = new Profiler(&TauMapFI, group, true); \
    TheTauMapFI(group) = &TauMapFI; \
    TauMapProf->Start(); \
    stmt; \
    TauMapProf->Stop(); \
  }

#else
// Multithreaded, we should use thread-safe tauCreateFI to create the FunctionInfo object
// Note: It's still not absolutely theoretically 100% thread-safe, since the static 
// initializer is not in a lock, but we don't want to pay that price for every function call 

#define TAU_MAPPING(stmt, group)   \
  { \
    static FunctionInfo *TauMapFI = NULL; \
    tauCreateFI(&TauMapFI, #stmt, " " , group, #group); \
    static Profiler *TauMapProf = new Profiler(TauMapFI, group, true); \
    TheTauMapFI(group) = TauMapFI; \
    TauMapProf->Start(); \
    stmt; \
    TauMapProf->Stop(); \
    delete TauMapProf; \
  } 

#endif

#define TAU_MAPPING_REGISTER(stmt, group)  { \
    static FunctionInfo *TauMapFI = NULL; \
    tauCreateFI(&TauMapFI,stmt, " " , group, #group); \
    TheTauMapFI(group) = TauMapFI; \
  } 

#define TAU_MAPPING_CREATE(name, type, key, groupname, tid)  { FunctionInfo *TauMapFI = new FunctionInfo(name, type, key, groupname, true, tid); \
    if (TauMapFI == (FunctionInfo *) NULL) { \
	printf("ERROR: new returns NULL"); exit(1); \
    } \
    TheTauMapFI(key) = TauMapFI; \
  } 
/* TAU_MAPPING_OBJECT creates a functionInfo pointer that may be stored in the 
   object that is used to relate a lower level layer with a higher level layer 
*/

#define TAU_MAPPING_CREATE1(name, type, key1, groupid, groupname, tid)  { FunctionInfo *TauMapFI = new FunctionInfo(name, type, groupid, groupname, true, tid); \
    if (TauMapFI == (FunctionInfo *) NULL) { \
	printf("ERROR: new returns NULL"); exit(1); \
    } \
    TheTauMapFI(key1) = TauMapFI; \
  } 
/* TAU_MAPPING_OBJECT creates a functionInfo pointer that may be stored in the 
   object that is used to relate a lower level layer with a higher level layer 
*/

/* TAU_MAPPING_TIMER_CREATE creates a functionInfo pointer with a specified 
   group name. */
#define TAU_MAPPING_TIMER_CREATE(t, name, type, gr, group_name) t = new FunctionInfo((string &) name, type, gr, group_name, true, RtsLayer::myThread());

#define TAU_MAPPING_OBJECT(FuncInfoVar) FunctionInfo * FuncInfoVar;

/* TAU_MAPPING_LINK gets in a var the function info object associated with the 
   given key (Group) 
*/
/*
This error should be reported when FuncInfoVar is NULL
	  //printf("ERROR: TAU_MAPPING_LINK map returns NULL FunctionInfo *\n"); \
There's no error when FunctionInfo * is NULL. A region may not be active.
*/
/* OLD --> did a return. Instead Profiler should check for Null. */
/*
#define TAU_MAPPING_LINK(FuncInfoVar, Group) FuncInfoVar = TheTauMapFI(Group); \
	if (FuncInfoVar == (FunctionInfo *)NULL) { \
 	  return; \
        } 
*/
#define TAU_MAPPING_LINK(FuncInfoVar, Group) FuncInfoVar = TheTauMapFI(Group); 

/* TAU_MAPPING_PROFILE profiles the entire routine by creating a profiler objeca
   and this behaves pretty much like TAU_PROFILE macro, except this gives in the
   FunctionInfo object pointer instead of name and type strings. 
*/
#define TAU_MAPPING_PROFILE(FuncInfoVar) Profiler FuncInfoVar##Prof(FuncInfoVar, FuncInfoVar != (FunctionInfo *) 0 ? FuncInfoVar->GetProfileGroup() : TAU_DEFAULT, false);

/* TAU_MAPPING_PROFILE_TIMER acts like TAU_PROFILE_TIMER by creating a profiler
   object that can be subsequently used with TAU_PROFILE_START and 
   TAU_PROFILE_STOP
*/
#define TAU_MAPPING_PROFILE_TIMER(Timer, FuncInfoVar, tid) Profiler *Timer; \
   Timer = new Profiler(FuncInfoVar,  FuncInfoVar != (FunctionInfo *) 0 ? FuncInfoVar->GetProfileGroup() : TAU_DEFAULT, true, tid); \
   if (Timer == (Profiler *) NULL) {\
     printf("ERROR: TAU_MAPPING_PROFILE_TIMER: new returns NULL Profiler *\n");\
   }
   

/* TAU_MAPPING_PROFILE_START acts like TAU_PROFILE_START by starting the timer 
*/
#define TAU_MAPPING_PROFILE_START(Timer, tid) Timer->Start(tid);

/* TAU_MAPPING_PROFILE_STOP acts like TAU_PROFILE_STOP by stopping the timer 
*/
#define TAU_MAPPING_PROFILE_STOP(tid) Profiler *cur = Profiler::CurrentProfiler[tid]; cur->Stop(tid); delete cur; 
#define TAU_MAPPING_PROFILE_EXIT(msg, tid)  Profiler::ProfileExit(msg, tid); 
#define TAU_MAPPING_DB_DUMP(tid)  Profiler::DumpData(tid); 
#define TAU_MAPPING_DB_PURGE(tid)  Profiler::PurgeData(tid); 
#define TAU_MAPPING_PROFILE_SET_NODE(node, tid)  RtsLayer::setMyNode(node, tid); 
#define TAU_MAPPING_PROFILE_SET_GROUP_NAME(timer, name) timer->SetPrimaryGroupName(name);
#define TAU_MAPPING_PROFILE_GET_GROUP_NAME(timer) timer->GetPrimaryGroup();
#define TAU_MAPPING_PROFILE_GET_GROUP(timer) timer->GetProfileGroup();
#define TAU_MAPPING_PROFILE_SET_NAME(timer, name) timer->SetName(name);
#define TAU_MAPPING_PROFILE_GET_NAME(timer) timer->GetName();
#define TAU_MAPPING_PROFILE_SET_TYPE(timer, name) timer->SetType(name);
#define TAU_MAPPING_PROFILE_GET_TYPE(timer) timer->GetType();
#define TAU_MAPPING_PROFILE_SET_GROUP(timer, id) timer->SetProfileGroup(id);

#else
/* Create null , except the main statement which should be executed as it is*/
#define TAU_MAPPING(stmt, group) stmt
#define TAU_MAPPING_OBJECT(FuncInfoVar) 
#define TAU_MAPPING_LINK(FuncInfoVar, Group) 
#define TAU_MAPPING_PROFILE(FuncInfoVar) 
#define TAU_MAPPING_CREATE(name, type, key, groupname, tid) 
#define TAU_MAPPING_PROFILE_TIMER(Timer, FuncInfoVar, tid)
#define TAU_MAPPING_TIMER_CREATE(t, name, type, gr, group_name)
#define TAU_MAPPING_PROFILE_START(Timer, tid) 
#define TAU_MAPPING_PROFILE_STOP(tid) 
#define TAU_MAPPING_PROFILE_EXIT(msg, tid)  
#define TAU_MAPPING_DB_DUMP(tid)
#define TAU_MAPPING_DB_PURGE(tid)
#define TAU_MAPPING_PROFILE_SET_NODE(node, tid)  
#define TAU_MAPPING_PROFILE_SET_GROUP_NAME(timer, name)
#define TAU_MAPPING_PROFILE_SET_NAME(timer, name) 
#define TAU_MAPPING_PROFILE_SET_TYPE(timer, name)
#define TAU_MAPPING_PROFILE_SET_GROUP(timer, id) 
#define TAU_MAPPING_PROFILE_GET_GROUP_NAME(timer) 
#define TAU_MAPPING_PROFILE_GET_GROUP(timer) 
#define TAU_MAPPING_PROFILE_GET_NAME(timer) 
#define TAU_MAPPING_PROFILE_GET_TYPE(timer) 

#endif /* PROFILING_ON or TRACING_ON  */
#endif /* _TAU_MAPPING_H_ */
