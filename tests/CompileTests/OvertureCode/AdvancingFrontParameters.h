#ifndef __ADVANCING_FRONT_PARAMETERS__
#define __ADVANCING_FRONT_PARAMETERS__

#include "Overture.h"
#include "DialogData.h"

class AdvancingFrontParameters
{
public:
  AdvancingFrontParameters( real maxang=80., real egrowth=-1.0, bool usefunc=true, int defAdvNum=-1 );
  AdvancingFrontParameters( const AdvancingFrontParameters &aparam ) : 
    maxNeighborAngle( aparam.maxNeighborAngle ), 
    edgeGrowthFactor( aparam.edgeGrowthFactor ), 
    useControlFunction( aparam.useControlFunction ),
    defaultNumberOfAdvances(aparam.defaultNumberOfAdvances) { }
  
  ~AdvancingFrontParameters() { }

  void setMaxNeighborAngle( real ang  )    { maxNeighborAngle = ang; }
  void setEdgeGrowthFactor( real fact )    { edgeGrowthFactor = fact; }
  void toggleControlFunction()   { useControlFunction = !useControlFunction ; }
  void setNumberOfAdvances( int adv ) { defaultNumberOfAdvances = adv; }

  real getMaxNeighborAngle() { return maxNeighborAngle; }
  real getEdgeGrowthFactor() { return edgeGrowthFactor; }
  bool usingControlFunction() { return useControlFunction; }
  int getNumberOfAdvances() { return defaultNumberOfAdvances; }

private: 
  real maxNeighborAngle;
  real edgeGrowthFactor;
  bool useControlFunction;
  int defaultNumberOfAdvances;
};

#endif
