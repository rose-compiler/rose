#ifndef BOUNDARY_CONDITION_PARAMETERS_H 
#define BOUNDARY_CONDITION_PARAMETERS_H 

#include "A++.h"
#include "OvertureTypes.h"

#ifndef OV_USE_DOUBLE
class floatMappedGridFunction;
#define REAL_MAPPED_GRID_FUNCTION floatMappedGridFunction
class floatGridCollectionFunction;
#define REAL_GRID_COLLECTION_FUNCTION floatGridCollectionFunction
#else
class doubleMappedGridFunction;
#define REAL_MAPPED_GRID_FUNCTION doubleMappedGridFunction
class doubleGridCollectionFunction;
#define REAL_GRID_COLLECTION_FUNCTION doubleGridCollectionFunction
#endif


// ===================================================================================
// This class is used to pass optional parameters to the boundary condition routines
// ===================================================================================
class BoundaryConditionParameters
{
public:

  enum CornerBoundaryConditionEnum
  {
    extrapolateCorner,
    symmetryCorner
  };

  enum BoundaryConditionForcingOption
  {
    scalarForcing=0,
    vectorForcing,
    arrayForcing,
    gridFunctionForcing
  };


  BoundaryConditionParameters();
  ~BoundaryConditionParameters();
  
  int lineToAssign;          // apply Dirichlet BC on this line
  int orderOfExtrapolation;
  int orderOfInterpolation;
  int ghostLineToAssign;     // assign this ghost line (various bc's)
  
  IntegerArray components;       // hold components for various BC's
  IntegerArray uComponents,fComponents;
  RealArray a,b0,b1,b2,b3;

  int interpolateRefinementBoundaries;  // if true, interpolate all refinement boundaries
  int interpolateHidden;                // if true, interpolate hidden coarse grid points from higher level refinemnts

  int setUseMask(int trueOrFalse=TRUE);
  int getUseMask() const{ return useMask;}
  
  intArray & mask();
  
  CornerBoundaryConditionEnum getCornerBoundaryCondition( int side1, int side2, int side3 = -1 ) const;
 
  int setCornerBoundaryCondition( CornerBoundaryConditionEnum bc );
  int setCornerBoundaryCondition( CornerBoundaryConditionEnum bc, int side1, int side2, int side3 = -1 );

  void setVariableCoefficients( REAL_MAPPED_GRID_FUNCTION & var ); // supply a grid function for variable coefficients
  void setVariableCoefficients( REAL_GRID_COLLECTION_FUNCTION & var ); 
  REAL_MAPPED_GRID_FUNCTION *getVariableCoefficients() const;
  REAL_MAPPED_GRID_FUNCTION *getVariableCoefficients(const int & grid) const;

  void setRefinementLevelToSolveFor( int level );
  int getRefinementLevelToSolveFor() const { return refinementLevelToSolveFor;}

  int setBoundaryConditionForcingOption( BoundaryConditionForcingOption option );
  BoundaryConditionForcingOption getBoundaryConditionForcingOption() const;
  

 protected:

  CornerBoundaryConditionEnum cornerBC[3][3][3]; // 0=extrapolate, 1=symmetry

  REAL_MAPPED_GRID_FUNCTION *variableCoefficients;
  REAL_GRID_COLLECTION_FUNCTION *variableCoefficientsGC;
  int useMask;
  intArray *maskPointer;   // for applying BC's selectively to points where mask != 0
  int refinementLevelToSolveFor;  // for refinement level solves.
  BoundaryConditionForcingOption boundaryConditionForcingOption;
  
};

#undef REAL_MAPPED_GRID_FUNCTION
#undef REAL_GRID_COLLECTION_FUNCTION

#endif
