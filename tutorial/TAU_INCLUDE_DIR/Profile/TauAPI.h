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
**	Description 	: TAU Profiling Package API			  **
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

#ifndef _TAU_API_H_
#define _TAU_API_H_

#if (defined(PROFILING_ON) || defined(TRACING_ON) )

//////////////////////////////////////////////////////////////////////
// TAU PROFILING API MACROS. 
// To ensure that Profiling does not add any runtime overhead when it 
// is turned off, these macros expand to null.
//////////////////////////////////////////////////////////////////////
extern "C" void Tau_start_timer(void * function_info, int phase );
extern "C" void Tau_stop_timer(void * function_info); 
extern "C" void Tau_create_top_level_timer_if_necessary(void);
extern "C" void Tau_stop_top_level_timer_if_necessary(void);
extern "C" char * Tau_phase_enable(const char *group);

#define TAU_TYPE_STRING(profileString, str) static string profileString(str);

#if (TAU_MAX_THREADS == 1)
// If we're not multi-threaded, just use the non-thread-safe static initializer

#define TAU_PROFILE(name, type, group) \
        static TauGroup_t tau_gr = group; \
        static FunctionInfo tauFI(name, type, tau_gr, #group); \
        Profiler tauFP(&tauFI, tau_gr);

#ifdef TAU_PROFILEPHASE
#define TAU_PHASE(name, type, group) \
        static TauGroup_t tau_group = group; \
	static char * TauGroupNameUsed = Tau_phase_enable(#group); \
        static FunctionInfo tauFInfo(name, type, tau_group, TauGroupNameUsed); \
        Profiler tauFProf(&tauFInfo, tau_group); \
	tauFProf.SetPhase(1);
#else /* TAU_PROFILEPHASE */
#define TAU_PHASE TAU_PROFILE
#endif /* TAU_PROFILEPHASE */

#define TAU_PROFILE_TIMER(var, name, type, group) \
        static TauGroup_t var##tau_gr = group; \
        static FunctionInfo *var##fi = new FunctionInfo(name, type, var##tau_gr, #group);

#define TAU_PROFILE_TIMER_DYNAMIC(var, name, type, group) \
        TauGroup_t var##tau_gr = group; \
        FunctionInfo *var##fi = new FunctionInfo(name, type, var##tau_gr, #group);

#ifdef TAU_PROFILEPHASE
#define TAU_PHASE_CREATE_STATIC(var, name, type, group) \
        static TauGroup_t var##tau_group = group; \
	static char * TauGroupNameUsed##var = Tau_phase_enable(#group); \
	static FunctionInfo *var##finfo = new FunctionInfo(name, type, var##tau_group, TauGroupNameUsed##var);

#define TAU_PHASE_CREATE_DYNAMIC(var, name, type, group) \
        TauGroup_t var##tau_group = group; \
	static char * TauGroupNameUsed##var = Tau_phase_enable(#group); \
	FunctionInfo *var##finfo = new FunctionInfo(name, type, var##tau_group,  TauGroupNameUsed##var); 

#define TAU_PHASE_START(var) if (var##tau_group & RtsLayer::TheProfileMask()) \
                                Tau_start_timer(var##finfo, 1);

#define TAU_PHASE_STOP(var)  if (var##tau_group & RtsLayer::TheProfileMask()) \
                                Tau_stop_timer(var##finfo);
#else /* TAU_PROFILEPHASE */
#define TAU_PHASE_CREATE_STATIC TAU_PROFILE_TIMER
#define TAU_PHASE_CREATE_DYNAMIC TAU_PROFILE_TIMER_DYNAMIC
#define TAU_PHASE_START TAU_PROFILE_START
#define TAU_PHASE_STOP  TAU_PROFILE_STOP
#endif /* TAU_PROFILEPHASE */



#else  /* TAU_MAX_THREADS */
// Multithreaded, we should use thread-safe tauCreateFI to create the FunctionInfo object
// Note: It's still not absolutely theoretically 100% thread-safe, since the static 
// initializer is not in a lock, but we don't want to pay that price for every function call 
#define TAU_PROFILE(name, type, group) \
	static TauGroup_t tau_gr = group; \
	static FunctionInfo *tauFI = NULL; \
        if (tauFI == 0) \
          tauCreateFI(&tauFI, name, type, tau_gr, #group); \
        Profiler tauFP(tauFI, tau_gr); 

#ifdef TAU_PROFILEPHASE
#define TAU_PHASE(name, type, group) \
	static TauGroup_t tau_group = group; \
	static FunctionInfo *tauFInfo = NULL; \
	static char * TauGroupNameUsed = Tau_phase_enable(#group); \
        tauCreateFI(&tauFInfo, name, type, tau_group, TauGroupNameUsed); \
	Profiler tauFProf(tauFInfo, tau_group); \
	tauFProf.SetPhase(1);
#else
#define TAU_PHASE TAU_PROFILE
#endif /* TAU_PROFILEPHASE */

#define TAU_PROFILE_TIMER(var, name, type, group) \
	static TauGroup_t var##tau_gr = group; \
	static FunctionInfo *var##fi = NULL; \
        if (var##fi == 0) \
          tauCreateFI(&var##fi, name, type, var##tau_gr, #group); 

#define TAU_PROFILE_TIMER_DYNAMIC(var, name, type, group) \
        TauGroup_t var##tau_gr = group; \
        FunctionInfo *var##fi = new FunctionInfo(name, type, var##tau_gr, #group);


#ifdef TAU_PROFILEPHASE
#define TAU_PHASE_CREATE_STATIC(var, name, type, group) \
	static TauGroup_t var##tau_group = group; \
	static FunctionInfo *var##finfo = NULL; \
	static char * TauGroupNameUsed##var = Tau_phase_enable(#group); \
        tauCreateFI(&var##finfo, name, type, var##tau_group, TauGroupNameUsed##var); 

#define TAU_PHASE_CREATE_DYNAMIC(var, name, type, group) \
	TauGroup_t var##tau_group = group; \
	FunctionInfo *var##finfo = NULL; \
	static char * TauGroupNameUsed##var = Tau_phase_enable(#group); \
        tauCreateFI(&var##finfo, name, type, var##tau_group, TauGroupNameUsed##var); 

#else
#define TAU_PHASE_CREATE_STATIC TAU_PROFILE_TIMER
#define TAU_PHASE_CREATE_DYNAMIC TAU_PROFILE_TIMER_DYNAMIC
#endif /* TAU_PROFILEPHASE */

// Construct a Profiler obj and a FunctionInfo obj with an extended name
// e.g., FunctionInfo loop1fi(); Profiler loop1(); 
#define TAU_PROFILE_START(var) if (var##tau_gr & RtsLayer::TheProfileMask()) \
 				Tau_start_timer(var##fi, 0);

#ifdef TAU_PROFILEPHASE
#define TAU_PHASE_START(var) if (var##tau_group & RtsLayer::TheProfileMask()) \
 				Tau_start_timer(var##finfo, 1);
#else
#define TAU_PHASE_START TAU_PROFILE_START
#endif /* TAU_PROFILEPHASE */

#ifdef TAU_PROFILEPHASE
#define TAU_PHASE_STOP(var)  if (var##tau_group & RtsLayer::TheProfileMask()) \
				Tau_stop_timer(var##finfo);
#else
#define TAU_PHASE_STOP TAU_PROFILE_STOP
#endif /* TAU_PROFILEPHASE */
#endif  /* TAU_MAX_THREADS */



// Construct a Profiler obj and a FunctionInfo obj with an extended name
// e.g., FunctionInfo loop1fi(); Profiler loop1();
#define TAU_PROFILE_START(var) if (var##tau_gr & RtsLayer::TheProfileMask()) \
                                Tau_start_timer(var##fi, 0);
#define TAU_PROFILE_STOP(var)  if (var##tau_gr & RtsLayer::TheProfileMask()) \
                                Tau_stop_timer(var##fi);

#define TAU_PROFILE_STMT(stmt) stmt;
#define TAU_PROFILE_EXIT(msg)  Profiler::ProfileExit(msg); 
#define TAU_PROFILE_INIT(argc, argv) RtsLayer::ProfileInit(argc, argv);
#define TAU_INIT(argc, argv) RtsLayer::ProfileInit(*argc, *argv);
#define TAU_PROFILE_SET_NODE(node) RtsLayer::setMyNode(node);
#define TAU_PROFILE_SET_CONTEXT(context) RtsLayer::setMyContext(context);
#define TAU_PROFILE_SET_GROUP_NAME(newname) tauFI.SetPrimaryGroupName(newname);
#define TAU_PROFILE_TIMER_SET_GROUP_NAME(t, newname) t##fi.SetPrimaryGroupName(newname);
#define TAU_PROFILE_TIMER_SET_NAME(t, newname)	t##fi.SetName(newname);
#define TAU_PROFILE_TIMER_SET_TYPE(t, newname)  t##fi.SetType(newname);
#define TAU_PROFILE_TIMER_SET_GROUP(t, id) t##fi.SetProfileGroup(id); 

#define TAU_GLOBAL_TIMER(timer, name, type, group) FunctionInfo& timer (void) { \
	static FunctionInfo *timer##fi = NULL; \
        tauCreateFI(&timer##fi, name, type, group, #group); \
	return *timer##fi; }

#define TAU_GLOBAL_TIMER_START(timer) { static FunctionInfo *timer##fptr= & timer (); \
	int tau_tid = RtsLayer::myThread(); \
	Profiler *t = new Profiler (timer##fptr, timer##fptr != (FunctionInfo *) 0 ? timer##fptr->GetProfileGroup() : TAU_DEFAULT, true, tau_tid); \
        t->Start(tau_tid); }

#define TAU_GLOBAL_TIMER_STOP()  {int tau_threadid = RtsLayer::myThread(); \
                Profiler *p = Profiler::CurrentProfiler[tau_threadid]; \
		p->Stop(tau_threadid); \
		delete p; \
		}

#define TAU_GLOBAL_TIMER_EXTERNAL(timer)  extern FunctionInfo& timer(void);

#ifdef TAU_PROFILEPHASE
#define TAU_GLOBAL_PHASE(timer, name, type, group) FunctionInfo& timer() { \
	static FunctionInfo *timer##fi = NULL; \
        tauCreateFI(&timer##fi, name, type, group, Tau_enable_phase(#group)); \
	return *timer##fi; }

#define TAU_GLOBAL_PHASE_START(timer) { static FunctionInfo *timer##fptr= & timer (); \
	int tau_tid = RtsLayer::myThread(); \
	Profiler *t = new Profiler (timer##fptr, timer##fptr != (FunctionInfo *) 0 ? timer##fptr->GetProfileGroup() : TAU_DEFAULT, true, tau_tid); \
	t->SetPhase(1); \
        t->Start(tau_tid); }

#define TAU_GLOBAL_PHASE_STOP(timer) Tau_stop_timer(&timer()); 

#define TAU_GLOBAL_PHASE_EXTERNAL(timer) extern FunctionInfo& timer(void);
#else /* TAU_PROFILEPHASE */
#define TAU_GLOBAL_PHASE 	TAU_GLOBAL_TIMER
#define TAU_GLOBAL_PHASE_START	TAU_GLOBAL_TIMER_START
#define TAU_GLOBAL_PHASE_STOP(timer)	TAU_GLOBAL_TIMER_STOP()
#define TAU_GLOBAL_PHASE_EXTERNAL TAU_GLOBAL_TIMER_EXTERNAL

#endif /* TAU_PROFILEPHASE */

/* The above macros are for use with global timers in a multi-threaded application */

#ifdef PROFILE_CALLSTACK
#define TAU_PROFILE_CALLSTACK()    Profiler::CallStackTrace();
#else
#define TAU_PROFILE_CALLSTACK() 
#endif /* PROFILE_CALLSTACK */

#define TAU_DB_DUMP() Profiler::DumpData();
#define TAU_DB_DUMP_PREFIX(prefix) Profiler::DumpData(false, RtsLayer::myThread(), prefix);
#define TAU_DB_DUMP_INCR() Profiler::DumpData(true);
#define TAU_DB_PURGE() Profiler::PurgeData();
#define TAU_GET_FUNC_NAMES(functionList, num) Profiler::theFunctionList(&functionList, &num);
#define TAU_DUMP_FUNC_NAMES() Profiler::dumpFunctionNames();
#ifdef TAU_MULTIPLE_COUNTERS
#define TAU_GET_COUNTER_NAMES(counterList, num) MultipleCounterLayer::theCounterList(&counterList, &num);
#else //TAU_MULTIPLE_COUNTERS
#define TAU_GET_COUNTER_NAMES(counterList, num) Profiler::theCounterList(&counterList, &num);
#endif //TAU_MULTIPLE_COUNTERS
#define TAU_GET_FUNC_VALS(v1,v2,v3,v4,v5,v6,v7,v8) \
                               Profiler::getFunctionValues(v1,v2,&v3,&v4,&v5,&v6,&v7,&v8);
#define TAU_DUMP_FUNC_VALS(v1,v2) \
                               Profiler::dumpFunctionValues(v1,v2);
#define TAU_DUMP_FUNC_VALS_INCR(v1,v2) \
                               Profiler::dumpFunctionValues(v1,v2,true);


// UserEvents

#define TAU_GET_EVENT_NAMES(eventList, num) Profiler::getUserEventList(&eventList, &num);
#define TAU_GET_EVENT_VALS(v1,v2,v3,v4,v5,v6,v7) \
                               Profiler::getUserEventValues(v1,v2,&v3,&v4,&v5,&v6,&v7);

#define TAU_REGISTER_EVENT(event, name)  	static TauUserEvent event(name);
#define TAU_EVENT(event, data) 		 	(event).TriggerEvent(data);
#define TAU_EVENT_SET_NAME(event, name) 	(event).SetEventName(name);
#define TAU_EVENT_DISABLE_MIN(event) 		(event).SetDisableMin(true);
#define TAU_EVENT_DISABLE_MAX(event) 		(event).SetDisableMax(true);
#define TAU_EVENT_DISABLE_MEAN(event) 		(event).SetDisableMean(true);
#define TAU_EVENT_DISABLE_STDDEV(event) 	(event).SetDisableStdDev(true);
#define TAU_REPORT_STATISTICS()			TauUserEvent::ReportStatistics();
#define TAU_REPORT_THREAD_STATISTICS()		TauUserEvent::ReportStatistics(true);

#define TAU_REGISTER_CONTEXT_EVENT(event, name) static TauContextUserEvent event(name);
#define TAU_CONTEXT_EVENT(event, data)		(event).TriggerEvent(data);
#define TAU_DISABLE_CONTEXT_EVENT(event)	(event).SetDisableContext(true);
#define TAU_ENABLE_CONTEXT_EVENT(event)		(event).SetDisableContext(false);



#define TAU_REGISTER_THREAD()			RtsLayer::RegisterThread();
#define TAU_REGISTER_FORK(id, op) 		RtsLayer::RegisterFork(id, op);
#define TAU_ENABLE_INSTRUMENTATION() 		RtsLayer::TheEnableInstrumentation() = true;
#define TAU_DISABLE_INSTRUMENTATION() 		RtsLayer::TheEnableInstrumentation() = false;
#define TAU_ENABLE_GROUP(group)			RtsLayer::enableProfileGroup(group)
#define TAU_DISABLE_GROUP(group)		RtsLayer::disableProfileGroup(group)
#define TAU_ENABLE_GROUP_NAME(group)		RtsLayer::enableProfileGroupName(group)
#define TAU_DISABLE_GROUP_NAME(group)		RtsLayer::disableProfileGroupName(group)
#define TAU_ENABLE_ALL_GROUPS()			RtsLayer::enableAllGroups()
#define TAU_DISABLE_ALL_GROUPS()		RtsLayer::disableAllGroups()
#define TAU_GET_PROFILE_GROUP(group)		RtsLayer::getProfileGroup(group)
#define TAU_ENABLE_TRACKING_MEMORY()		TauEnableTrackingMemory()
#define TAU_DISABLE_TRACKING_MEMORY()		TauDisableTrackingMemory()
#define TAU_ENABLE_TRACKING_MEMORY_HEADROOM()	TauEnableTrackingMemoryHeadroom()
#define TAU_DISABLE_TRACKING_MEMORY_HEADROOM()	TauDisableTrackingMemoryHeadroom()
#define TAU_TRACK_MEMORY()			TauTrackMemoryUtilization(true)
#define TAU_TRACK_MEMORY_HEADROOM()		TauTrackMemoryUtilization(false)
#define TAU_TRACK_MEMORY_HERE()			TauTrackMemoryHere()
#define TAU_TRACK_MEMORY_HEADROOM_HERE()	TauTrackMemoryHeadroomHere()
#define TAU_ENABLE_TRACKING_MUSE_EVENTS()	TauEnableTrackingMuseEvents()
#define TAU_DISABLE_TRACKING_MUSE_EVENTS()	TauDisableTrackingMuseEvents()
#define TAU_TRACK_MUSE_EVENTS()			TauTrackMuseEvents()
#define TAU_SET_INTERRUPT_INTERVAL(value)	TauSetInterruptInterval(value)


#ifdef NO_RTTI
/* #define CT(obj) string(#obj) */
#define CT(obj) string(" ")
#else // RTTI is present
//#define CT(obj) string(RtsLayer::CheckNotNull(typeid(obj).name())) 
#define CT(obj) RtsLayer::GetRTTI(typeid(obj).name())
#endif //NO_RTTI

#else /* PROFILING_ON */
/* In the absence of profiling, define the functions as null */
#define TYPE_STRING(profileString, str)
#define PROFILED_BLOCK(name, type) 

#define TAU_TYPE_STRING(profileString, str) 
#define TAU_PROFILE(name, type, group) 
#define TAU_PROFILE_TIMER(var, name, type, group)
#define TAU_PROFILE_START(var)
#define TAU_PROFILE_STOP(var)
#define TAU_PROFILE_STMT(stmt) 
#define TAU_PROFILE_EXIT(msg)
#define TAU_PROFILE_INIT(argc, argv)
#define TAU_INIT(argc, argv) 
#define TAU_PROFILE_SET_NODE(node)
#define TAU_PROFILE_SET_CONTEXT(context)
#define TAU_PROFILE_SET_GROUP_NAME(newname)
#define TAU_PROFILE_TIMER_SET_GROUP_NAME(t, newname)
#define TAU_PROFILE_TIMER_SET_NAME(t, newname)
#define TAU_PROFILE_TIMER_SET_TYPE(t, newname)
#define TAU_PROFILE_TIMER_SET_GROUP(t, id)
#define TAU_PROFILE_CALLSTACK()    
#define TAU_DB_DUMP()
#define TAU_DB_DUMP_PREFIX(prefix) 
#define TAU_DB_DUMP_INCR()
#define TAU_DB_PURGE()
#define TAU_GET_FUNC_NAMES(functionList, num)
#define TAU_DUMP_FUNC_NAMES()
#define TAU_GET_COUNTER_NAMES(counterList, num)
#define TAU_GET_FUNC_VALS(v1,v2,v3,v4,v5,v6,v7,v8)
#define TAU_DUMP_FUNC_VALS(v1,v2)
#define TAU_DUMP_FUNC_VALS_INCR(v1,v2)

#define TAU_REGISTER_EVENT(event, name)
#define TAU_EVENT(event, data)
#define TAU_EVENT_SET_NAME(event, name) 	
#define TAU_EVENT_DISABLE_MIN(event)
#define TAU_EVENT_DISABLE_MAX(event)
#define TAU_EVENT_DISABLE_MEAN(event)
#define TAU_EVENT_DISABLE_STDDEV(event)
#define TAU_REPORT_STATISTICS()
#define TAU_REPORT_THREAD_STATISTICS()
#define TAU_REGISTER_THREAD()
#define TAU_REGISTER_FORK(id, op) 
#define TAU_ENABLE_INSTRUMENTATION() 		
#define TAU_DISABLE_INSTRUMENTATION() 	
#define TAU_ENABLE_GROUP(group)
#define TAU_DISABLE_GROUP(group)
#define TAU_ENABLE_GROUP_NAME(group)
#define TAU_DISABLE_GROUP_NAME(group)
#define TAU_ENABLE_ALL_GROUPS()			
#define TAU_DISABLE_ALL_GROUPS()	
#define TAU_ENABLE_TRACKING_MEMORY()
#define TAU_DISABLE_TRACKING_MEMORY()
#define TAU_TRACK_MEMORY()
#define TAU_TRACK_MEMORY_HERE()
#define TAU_ENABLE_TRACKING_MUSE_EVENTS()	
#define TAU_DISABLE_TRACKING_MUSE_EVENTS()
#define TAU_TRACK_MUSE_EVENTS()		
#define TAU_SET_INTERRUPT_INTERVAL(value)

#define TAU_PHASE_CREATE_STATIC(var, name, type, group) 
#define TAU_PHASE_CREATE_DYNAMIC(var, name, type, group) 
#define TAU_PHASE_START(var) 
#define TAU_PHASE_STOP(var) 
#define TAU_GLOBAL_PHASE(timer, name, type, group) 
#define TAU_GLOBAL_PHASE_START(timer) 
#define TAU_GLOBAL_PHASE_STOP(timer)  
#define TAU_GLOBAL_PHASE_EXTERNAL(timer) 
#define TAU_GLOBAL_TIMER(timer, name, type, group)
#define TAU_GLOBAL_TIMER_EXTERNAL(timer)
#define TAU_GLOBAL_TIMER_START(timer)
#define TAU_GLOBAL_TIMER_STOP()

#define TAU_REGISTER_CONTEXT_EVENT(event, name)
#define TAU_CONTEXT_EVENT(event, data)
#define TAU_DISABLE_CONTEXT_EVENT(event)
#define TAU_ENABLE_CONTEXT_EVENT(event)



#define CT(obj)

#endif /* PROFILING_ON */

#ifdef TRACING_ON
#define TAU_TRACE_SENDMSG(type, destination, length) \
	RtsLayer::TraceSendMsg(type, destination, length); 
#define TAU_TRACE_RECVMSG(type, source, length) \
	RtsLayer::TraceRecvMsg(type, source, length); 

#else /* TRACING_ON */
#define TAU_TRACE_SENDMSG(type, destination, length) 
#define TAU_TRACE_RECVMSG(type, source, length)
#endif /* TRACING_ON */


#ifdef DEBUG_PROF
#define DEBUGPROFMSG(msg) { cout<< msg; }
#else
#define DEBUGPROFMSG(msg) 
#endif // DEBUG_PROF

#endif /* _TAU_API_H_ */
/***************************************************************************
 * $RCSfile: TauAPI.h,v $   $Author: dquinlan $
 * $Revision: 1.1 $   $Date: 2006/04/24 00:22:01 $
 * POOMA_VERSION_ID: $Id: TauAPI.h,v 1.1 2006/04/24 00:22:01 dquinlan Exp $ 
 ***************************************************************************/
