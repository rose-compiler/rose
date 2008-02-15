#ifndef BCTypes_H 
#define BCTypes_H

// The only purpose of this class is to hold the names of the 
// Elementary Boundary Condition Types
class BCTypes
{
public:
  enum BCNames  // **** remember to also change GenericMappedGridOperators.h ****
  {
    dirichlet,
    neumann,
    extrapolate,
    normalComponent,
    mixed,
    generalMixedDerivative,
    normalDerivativeOfNormalComponent,
    normalDerivativeOfADotU,
    aDotU,
    aDotGradU,
    normalDotScalarGrad,
    evenSymmetry,
    generalizedDivergence,
    vectorSymmetry,
    tangentialComponent0,
    tangentialComponent1,
    normalDerivativeOfTangentialComponent0,
    normalDerivativeOfTangentialComponent1,
    extrapolateInterpolationNeighbours,
    tangentialComponent,                      
    extrapolateNormalComponent,
    extrapolateTangentialComponent0,
    extrapolateTangentialComponent1,
    userDefinedBoundaryCondition0,
    userDefinedBoundaryCondition1,
    userDefinedBoundaryCondition2,
    userDefinedBoundaryCondition3,
    userDefinedBoundaryCondition4,
    userDefinedBoundaryCondition5,
    userDefinedBoundaryCondition6,
    userDefinedBoundaryCondition7,
    userDefinedBoundaryCondition8,
    userDefinedBoundaryCondition9,
    extrapolateRefinementBoundaries,
    numberOfDifferentBoundaryConditionTypes,   // counts number of entries in this list
    allBoundaries=-99999,                      // default argument
    boundary1=allBoundaries+1,                 // (side,axis)=(0,0)
    boundary2=boundary1+1,                     // (side,axis)=(1,0)
    boundary3=boundary2+1,                     // (side,axis)=(0,1)
    boundary4=boundary3+1,                     // (side,axis)=(1,1)
    boundary5=boundary4+1,                     // (side,axis)=(0,2)
    boundary6=boundary5+1                      // (side,axis)=(1,2)
    };
  BCTypes() {}
  ~BCTypes() {}
  
  // return the BCNames values for a given face of a grid (side,axis)
  static BCNames boundary(int side, int axis) {return BCNames(boundary1+side+2*axis); } 
  

};


#endif
