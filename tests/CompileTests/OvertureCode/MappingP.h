#ifndef MAPPINGP_H
#define MAPPINGP_H "MappingP.h"

#include <A++.h>

#include "OvertureTypes.h"     // define real to be float or double

#include "wdhdefs.h"           // some useful defines and constants
#include "MappingWS.h"

class ApproximateGlobalInverse;    //  forward declaration
class ExactLocalInverse;

//===========================================================================
// Here is a class to use to pass parameters to inverseMap
//    ---temporarily located here---
//===========================================================================
class MappingParameters
{
 public:
  enum realParameter  
  {
    THEnonConvergenceValue,    // value given to inverse when there is no convergence
    THEnewtonToleranceFactor,  // convergence tolerance is this times the machine epsilon
    THEnewtonDivergenceValue,  // newton is deemed to have diverged if the r value is this much outside [0,1]
    THEboundingBoxExtensionFactor, // relative amount to increase the bounding box each direction.
    THEstencilWalkBoundingBoxExtensionFactor   // increase bounding box for the stencil walk.
  };
  enum intParameter  
  {
    THEfindBestGuess  // if true, ignore bounding boxes when finding closest point
  };

  int isNull;                   // True if parameters have not been set
  int periodicityOfSpace;       // =0,1,2,3
  RealArray periodicityVector;
  MappingWorkSpace workSpace;
  int computeGlobalInverse;     
  // This next int should really be of type Mapping::coordinateSystem
  int coordinateType;       // to request the mapping transformation to be evaluated
                            // in one of the coordinate systems: cartesian, spherical,
                            // cylindrical, polar, toroidal

  intArray mask;            // optional mask for limiting which points are evaluated by map and inverseMap
  
  ApproximateGlobalInverse *approximateGlobalInverse;  // 
  ExactLocalInverse *exactLocalInverse;

  MappingParameters( const int isNull0=FALSE );

  ~MappingParameters();

  // Copy constructor is deep by default
  MappingParameters( const MappingParameters &, const CopyType copyType=DEEP );

  MappingParameters & operator =( const MappingParameters & X );

};

#endif   // MAPPINGP_H
