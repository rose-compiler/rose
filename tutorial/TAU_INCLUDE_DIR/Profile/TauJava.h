/****************************************************************************
**                      TAU Portable Profiling Package                     **
**                      http://www.acl.lanl.gov/tau                        **
*****************************************************************************
**    Copyright 1997                                                       **
**    Department of Computer and Information Science, University of Oregon **
**    Advanced Computing Laboratory, Los Alamos National Laboratory        **
****************************************************************************/
/***************************************************************************
**      File            : TauJava.h                                       **
**      Description     : TAU interface for JVMPI                         **
**      Author          : Sameer Shende                                   **
**      Contact         : sameer@cs.uoregon.edu sameer@acl.lanl.gov       **
**      Flags           : Compile with                                    **
**                        -DPROFILING_ON to enable profiling (ESSENTIAL)  **
**                        -DPROFILE_STATS for Std. Deviation of Excl Time **
**                        -DSGI_HW_COUNTERS for using SGI counters        **
**                        -DPROFILE_CALLS  for trace of each invocation   **
**                        -DSGI_TIMERS  for SGI fast nanosecs timer       **
**                        -DTULIP_TIMERS for non-sgi Platform             **
**                        -DPOOMA_STDSTL for using STD STL in POOMA src   **
**                        -DPOOMA_TFLOP for Intel Teraflop at SNL/NM      **
**                        -DPOOMA_KAI for KCC compiler                    **
**                        -DDEBUG_PROF  for internal debugging messages   **
**                        -DPROFILE_CALLSTACK to enable callstack traces  **
**      Documentation   : See http://www.acl.lanl.gov/tau                 **
***************************************************************************/

//////////////////////////////////////////////////////////////////////
// Declarations 
//////////////////////////////////////////////////////////////////////

#ifndef _TAU_JAVA_H_
#define _TAU_JAVA_H_

struct TauJavaLayer {
  static void Init(char *options);
  static void NotifyEvent(JVMPI_Event *event);
  static void ClassLoad(JVMPI_Event *event);
  static void MethodEntry(JVMPI_Event *event);
  static void MethodExit(JVMPI_Event *event);
  static void ThreadStart(JVMPI_Event *event);
  static void ThreadEnd(JVMPI_Event *event);
  static void ShutDown(JVMPI_Event *event);
  static void DataDump(JVMPI_Event *event);
  static void DataPurge(JVMPI_Event *event);
  static int *RegisterThread(JVMPI_Event *event);
  static void CreateTopLevelRoutine(char *name, char *type, char *groupname, 
                        int tid);
  static int  GetTid(JVMPI_Event *event);
  static int  NumThreads;
};


extern "C" {
  JNIEXPORT jint JNICALL JVM_OnLoad(JavaVM *jvm, char *options, void *reserved);
}

#endif /* _TAU_JAVA_H_ */
