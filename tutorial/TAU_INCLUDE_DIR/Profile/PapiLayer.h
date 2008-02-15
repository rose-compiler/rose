/////////////////////////////////////////////////
//Class definintion file for the Papi class.
//
//Author:   Robert Bell
//Created:  February 2000
//
/////////////////////////////////////////////////

#ifndef _PAPI_LAYER_H_
#define _PAPI_LAYER_H_

#ifdef TAU_PAPI
extern "C" {
/*#include "papiStdEventDefs.h" */
#include "papi.h"
/*
#include "papi_internal.h"
*/
}


  struct ThreadValue{
  int ThreadID;
  int EventSet; 
  long long *CounterValues;
  };



class PapiLayer
{
  public:
  static int map_eventnames(char *name);
  static int PapiLayerInit(bool lock = true);
  static void multiCounterPapiInit(void);
  static long long getCounters(int tid);
  static long long getWallClockTime(void);
  static long long getVirtualTime(void);
};

#endif /* TAU_PAPI */
#endif /* _PAPI_LAYER_H_ */

/////////////////////////////////////////////////
//
//End Papi class definition.
//
/////////////////////////////////////////////////




