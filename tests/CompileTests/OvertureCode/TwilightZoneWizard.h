/*  -*-Mode: c++; -*-  */
#ifndef TWILIGHT_ZONE_WIZARD_H
#define TWILIGHT_ZONE_WIZARD_H

#include "OGPolyFunction.h"
#include "OGTrigFunction.h"
#include "testUtils.h"
#include "davidsReal.h"

  enum TwilightZoneFlowFunctionType
  {
    none = 0,
    PolyFunction,
    TrigFunction,
    numberOfTwilightZoneFlowFunctionTypes
  };



OGFunction* 
setTwilightZoneFlowFunction (const TwilightZoneFlowFunctionType & TZType, 
                             const int & numberOfDimensions);


OGFunction*
setTwilightZoneFlowFunction (const int & numberOfDimensions);

#endif
