/////////////////////////////////////////////////
//Class definintion file for the PCL_Layer class.
//
//Author:   Robert Ansell-Bell
//Created:  July 1999
//
/////////////////////////////////////////////////

#ifndef _PCL_LAYER_H_
#define _PCL_LAYER_H_

#ifdef TAU_PCL
#include "pcl.h"


  struct ThreadValue{
  int ThreadID;
  PCL_FP_CNT_TYPE CounterValue;
  };



class PCL_Layer
{

  //No need to define constructors and destructors.
  //The default ones will do.

  public:
    //Default getCounters.
  static int map_eventnames(char *name);
  static int PCLLayerInit(PCL_DESCR_TYPE *descr);
  static void multiCounterPCLInit(PCL_DESCR_TYPE *descr);
  static PCL_FP_CNT_TYPE getCounters(int tid);
};

#endif /* TAU_PCL */
#endif /* _PCL_LAYER_H_ */

/////////////////////////////////////////////////
//
//End PCL_Layer class definition.
//
/////////////////////////////////////////////////




