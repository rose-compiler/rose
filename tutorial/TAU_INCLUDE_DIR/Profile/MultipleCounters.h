/////////////////////////////////////////////////
//Header file for the multiple counter layer.
//
//Author:   Robert Bell
//Created:  January 2002
//
/////////////////////////////////////////////////

#ifndef _MULTIPLE_COUNTER_LAYER_H_
#define _MULTIPLE_COUNTER_LAYER_H_

#ifdef TAU_MULTIPLE_COUNTERS


//If we are going to use Papi, include its headers.
#ifdef TAU_PAPI
extern "C" {
  /*#include "papiStdEventDefs.h" */
#include "papi.h"
  /*
#include "papi_internal.h"
  */
}
#endif /* TAU_PAPI */


#define SIZE_OF_INIT_ARRAY 12 //!!Change this if functions are added to the system!!


//Some useful typedefs
typedef bool (*firstListType)(int);
typedef void (*secondListType)(int, double[]);

class MultipleCounterLayer
{
 public:
  static bool initializeMultiCounterLayer(void);
  static bool * getCounterUsedList();
  static bool getCounterUsed(int inPosition);
  static int  getNumberOfCountersUsed(void);
  static void setCounterUsed(bool inValue, int inPosition);
  static void getCounters(int tid, double values[]);
  static char * getCounterNameAt(int position);
  static void theCounterList(const char ***inPtr, int *numOfCounters);
  static void theCounterListInternal(const char ***inPtr, int *numOfCounters, bool **tmpPtr);
#ifdef TRACING_ON
  static void triggerCounterEvents(unsigned long long timestamp, double *values, int tid);
#endif /* TRACING_ON */
  //*********************
  //The list of counter functions, and their init. functions.
  //Please see the help files on multiple
  //counters to see our conventions.
  //*********************
  
 private:
  static bool gettimeofdayMCLInit(int functionPosition);
  static void gettimeofdayMCL(int tid, double values[]);

  static bool linuxTimerMCLInit(int functionPosition);
  static void linuxTimerMCL(int tid, double values[]);

  static bool sgiTimersMCLInit(int functionPosition);
  static void sgiTimersMCL(int tid, double values[]);

  static bool crayTimersMCLInit(int functionPosition);
  static void crayTimersMCL(int tid, double values[]);

  static bool cpuTimeMCLInit(int functionPosition);
  static void cpuTimeMCL(int tid, double values[]);

  static bool javaCpuTimeMCLInit(int functionPosition);
  static void javaCpuTimeMCL(int tid, double values[]);

  static bool tauMUSEMCLInit(int functionPosition);
  static void tauMUSEMCL(int tid, double values[]);
  
  static bool tauMPIMessageSizeMCLInit(int functionPosition);
  static void tauMPIMessageSizeMCL(int tid, double values[]);
  
  static bool papiMCLInit(int functionPosition);
  static void papiMCL(int tid, double values[]);

  static bool papiWallClockMCLInit(int functionPosition);
  static void papiWallClockMCL(int tid, double values[]);

  static bool papiVirtualMCLInit(int functionPosition);
  static void papiVirtualMCL(int tid, double values[]);

  static bool pclMCLInit(int functionPosition);
  static void pclMCL(int tid, double values[]);
  //*********************
  //End - List of counter and init. functions.
  //*********************

  //Other class stuff.
  static char environment[25][10];

  static int gettimeofdayMCL_CP[1];
  static int gettimeofdayMCL_FP;

#ifdef TAU_LINUX_TIMERS  
  static int linuxTimerMCL_CP[1];
  static int linuxTimerMCL_FP;
#endif //TAU_LINUX_TIMERS

#ifdef SGI_TIMERS
  static int sgiTimersMCL_CP[1];
  static int sgiTimersMCL_FP;
#endif // SGI_TIMERS

#ifdef CRAY_TIMERS
  static int crayTimersMCL_CP[1];
  static int crayTimersMCL_FP;
#endif // CRAY_TIMERS

#ifdef CPU_TIME
  static int cpuTimeMCL_CP[1];
  static int cpuTimeMCL_FP;
#endif // CPU_TIME

#ifdef JAVA_CPU_TIME
  static int javaCpuTimeMCL_CP[1];
  static int javaCpuTimeMCL_FP;
#endif // JAVA_CPU_TIME

#ifdef TAU_MUSE
  static int tauMUSEMCL_CP[1];
  static int tauMUSEMCL_FP;
#endif // TAU_MUSE

#ifdef TAU_MPI
  static int tauMPIMessageSizeMCL_CP[1];
  static int tauMPIMessageSizeMCL_FP;
#endif // TAU_MPI
  
#ifdef TAU_PAPI
  static int papiMCL_CP[MAX_TAU_COUNTERS];
  static int papiWallClockMCL_CP[1];
  static int papiVirtualMCL_CP[1];
  static int papiMCL_FP; 
  static int papiWallClockMCL_FP;
  static int papiVirtualMCL_FP;
  //Data specific to the papiMCL function.
  static int numberOfPapiHWCounters;
  static int PAPI_CounterCodeList[MAX_TAU_COUNTERS];
  static ThreadValue * ThreadList[TAU_MAX_THREADS];
#endif//TAU_PAPI

#ifdef TAU_PCL
  static int pclMCL_CP[MAX_TAU_COUNTERS];
  static int pclMCL_FP;
  //Data specific to the pclMCL function.
  static int numberOfPCLHWCounters;
  static int PCL_CounterCodeList[MAX_TAU_COUNTERS];
  static unsigned int PCL_Mode;
  static PCL_DESCR_TYPE descr;
  static bool threadInit[TAU_MAX_THREADS];
  static PCL_CNT_TYPE CounterList[MAX_TAU_COUNTERS];
  static PCL_FP_CNT_TYPE FpCounterList[MAX_TAU_COUNTERS];
#endif//TAU_PCL

  static firstListType initArray[SIZE_OF_INIT_ARRAY];
  static secondListType functionArray[MAX_TAU_COUNTERS];
  static int numberOfActiveFunctions;
  static char * names[MAX_TAU_COUNTERS];
  static bool counterUsed[MAX_TAU_COUNTERS];
  static int numberOfCounters[MAX_TAU_COUNTERS];
#ifdef TRACING_ON
  static TauUserEvent **counterEvents;
#endif /* TRACING_ON */
};

#endif /* TAU_MULTIPLE_COUNTERS */
#endif /* _MULTIPLE_COUNTER_LAYER_H_ */

/////////////////////////////////////////////////
//
//End - Multiple counter layer.
//
/////////////////////////////////////////////////
