#ifndef MAPPING_PROJECTION_PARAMETERS_H
#define MAPPING_PROJECTION_PARAMETERS_H

#include "Mapping.h"

//--------------------------------------------------------------------------
//  This class holds parameters for the Mapping project function.
// 
//   It will hold state values that depend on the type of Mapping being projected,
// either a standard Mapping or a CompositeSurface
//--------------------------------------------------------------------------
class MappingProjectionParameters
{
 public:

  enum IntegerArrayName
  {
    subSurfaceIndex=0,
    ignoreThisSubSurface=1,
    numberOfIntegerArrayParameters=2
  };
  enum RealArrayName
  {
    r=0,
    x=1,
    xr=2,
    normal=3,
    numberOfRealArrayParameters=4
  };


  MappingProjectionParameters();
  ~MappingProjectionParameters();

  MappingProjectionParameters & operator =( const MappingProjectionParameters & x );

  // indicate whether the projection is being used with a marching algorithm
  int setIsAMarchingAlgorithm(const bool & trueOrFalse =TRUE );
  bool isAMarchingAlgorithm() const;

  int reset();

  // for tree search on unstructured grids:
  int setSearchBoundingBoxSize( real estimated, real maximumAllowed=0. );
  
  IntegerDistributedArray & getIntArray(const IntegerArrayName & name);

  RealDistributedArray & getRealArray(const RealArrayName & name);

  real searchBoundingBoxSize, searchBoundingBoxMaximumSize;

 private:

  bool marching;
  // All the arrays are saved in the following two arrays of pointers
  intArray *integerArrayParameter[numberOfIntegerArrayParameters];
  realArray *realArrayParameter[numberOfRealArrayParameters];

};


#endif  
