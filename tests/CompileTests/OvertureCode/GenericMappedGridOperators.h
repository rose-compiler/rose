/*  -*-Mode: c++; -*-  */

#ifndef GENERIC_MAPPED_GRID_OPERATORS
#define GENERIC_MAPPED_GRID_OPERATORS

#include "Overture.h"
#include "OGFunction.h"
#include "BoundaryConditionParameters.h"
#include "BCTypes.h"
#include "GridFunctionParameters.h"
#include "OvertureTypes.h"

// extern BoundaryConditionParameters Overture::defaultBoundaryConditionParameters();


//===========================================================================================
// This class is the base class for MappedGridOperators
//
//  o To define the derivatives in a different way you should derive from this class
//    and redefine any functions that you want to. If you provide the virtualConstructor
//    member function then your derived class can be used by the GridCollectionOperators
//    and CompositeGridOperators classes which define derivatives for GridCollectionFunction's
//    and CompositeGridFunction's.
//
//  Who to blame: Bill Henshaw, CIC-19, henshaw@lanl.gov
//===========================================================================================

class GenericMappedGridOperators
{

 public:

  enum AveragingType  
  {
    arithmeticAverage,
    harmonicAverage
  };

  enum         
  {
    forAll=realMappedGridFunction::forAll,
    undefinedValue=realMappedGridFunction::undefinedValue,
    allBoundaries=BCTypes::allBoundaries,
    spectral=-999   // for order of accuracy
  };
  
  MappedGrid mappedGrid;           // operators are defined for this MappedGrid

  // ********************************************************************
  // **************** Miscellaneous Functions **************************
  // ********************************************************************

  // default constructor
  GenericMappedGridOperators();
  // contructor taking a MappedGrid
  GenericMappedGridOperators( MappedGrid & mg );                  
  // copy constructor
  GenericMappedGridOperators( const GenericMappedGridOperators & mgo );  
  // create a new object of this class
  virtual GenericMappedGridOperators* virtualConstructor() const;        
  
  virtual ~GenericMappedGridOperators();

  virtual GenericMappedGridOperators & operator= ( const GenericMappedGridOperators & mgo );

  // return size of this object  
  virtual real sizeOf(FILE *file = NULL ) const;

  // supply a new grid to use
  virtual void updateToMatchGrid( MappedGrid & mg );         
  
  // get from a database file
  virtual int get( const GenericDataBase & dir, const aString & name);  
  // put to a database file
  virtual int put( GenericDataBase & dir, const aString & name) const;  

  // set the order of accuracy
  virtual void setOrderOfAccuracy( const int & orderOfAccuracy );
  
  // return true if the grid is rectangular
  virtual bool isRectangular();

  // This function is used to evaluate a whole set of derivatives at a time (for efficiency)
  virtual void getDerivatives(const realMappedGridFunction & u, 
			      const Index & I1 = nullIndex, 
			      const Index & I2 = nullIndex, 
			      const Index & I3 = nullIndex, 
			      const Index & I4 = nullIndex,
                              const Index & Evalute = nullIndex );   // evaluate these

  // Indicate the stencil size for functions returning coefficients
  virtual void setStencilSize(const int stencilSize);

  // Indicate the number of components (system size) for functions returning coefficients
  virtual void setNumberOfComponentsForCoefficients(const int number);

  // Indicate if twilight-zone forcing should be added to boundary conditions  
  virtual void setTwilightZoneFlow( const int & twilightZoneFlow );

  // Supply a twilight-zone forcing to use for boundary conditions  
  virtual void setTwilightZoneFlowFunction( OGFunction & twilightZoneFlowFunction );



  // ************************************************
  // ***** DIFFERENTIATION CLASS FUNCTIONS **********
  // ************************************************


// Macro to define a typical function call
#define FUNCTION(type) \
  virtual realMappedGridFunction type(const realMappedGridFunction & u,  \
				      const Index & I1 = nullIndex,      \
				      const Index & I2 = nullIndex,      \
				      const Index & I3 = nullIndex,      \
				      const Index & I4 = nullIndex,      \
				      const Index & I5 = nullIndex,      \
				      const Index & I6 = nullIndex,      \
				      const Index & I7 = nullIndex,      \
				      const Index & I8 = nullIndex );    \
                                                                         \
  virtual realMappedGridFunction type(const realMappedGridFunction & u,  \
                                      const GridFunctionParameters & gfType,   \
				      const Index & I1 = nullIndex,      \
				      const Index & I2 = nullIndex,      \
				      const Index & I3 = nullIndex,      \
				      const Index & I4 = nullIndex,      \
				      const Index & I5 = nullIndex,      \
				      const Index & I6 = nullIndex,      \
				      const Index & I7 = nullIndex,      \
				      const Index & I8 = nullIndex );     


#define FUNCTION_COEFFICIENTS(type) \
  virtual realMappedGridFunction type(const Index & I1 = nullIndex,      \
				      const Index & I2 = nullIndex,      \
				      const Index & I3 = nullIndex,      \
				      const Index & I4 = nullIndex,      \
				      const Index & I5 = nullIndex,      \
				      const Index & I6 = nullIndex,      \
				      const Index & I7 = nullIndex,      \
				      const Index & I8 = nullIndex );    \
                                                                         \
  virtual realMappedGridFunction type(const GridFunctionParameters & gfType,   \
                                      const Index & I1 = nullIndex,      \
				      const Index & I2 = nullIndex,      \
				      const Index & I3 = nullIndex,      \
				      const Index & I4 = nullIndex,      \
				      const Index & I5 = nullIndex,      \
				      const Index & I6 = nullIndex,      \
				      const Index & I7 = nullIndex,      \
				      const Index & I8 = nullIndex );

  // parametric derivatives in the r1,r2,r3 directions
  FUNCTION(r1)
  FUNCTION_COEFFICIENTS(r1Coefficients)
  FUNCTION(r2)
  FUNCTION_COEFFICIENTS(r2Coefficients)
  FUNCTION(r3)
  FUNCTION_COEFFICIENTS(r3Coefficients)
  FUNCTION(r1r1)
  FUNCTION_COEFFICIENTS(r1r1Coefficients)
  FUNCTION(r1r2)
  FUNCTION_COEFFICIENTS(r1r2Coefficients)
  FUNCTION(r1r3)
  FUNCTION_COEFFICIENTS(r1r3Coefficients)
  FUNCTION(r2r2)
  FUNCTION_COEFFICIENTS(r2r2Coefficients)
  FUNCTION(r2r3)
  FUNCTION_COEFFICIENTS(r2r3Coefficients)
  FUNCTION(r3r3)
  FUNCTION_COEFFICIENTS(r3r3Coefficients)

  // FUNCTIONs in the x,y,z directions
  FUNCTION(x)
  FUNCTION_COEFFICIENTS(xCoefficients)
  FUNCTION(y)
  FUNCTION_COEFFICIENTS(yCoefficients)
  FUNCTION(z)
  FUNCTION_COEFFICIENTS(zCoefficients)
  FUNCTION(xx)
  FUNCTION_COEFFICIENTS(xxCoefficients)
  FUNCTION(xy)
  FUNCTION_COEFFICIENTS(xyCoefficients)
  FUNCTION(xz)
  FUNCTION_COEFFICIENTS(xzCoefficients)
  FUNCTION(yy)
  FUNCTION_COEFFICIENTS(yyCoefficients)
  FUNCTION(yz)
  FUNCTION_COEFFICIENTS(yzCoefficients)
  FUNCTION(zz)
  FUNCTION_COEFFICIENTS(zzCoefficients)

  // other forms of derivatives

 // compute face-centered variable from cell-centered variable 
  FUNCTION(cellsToFaces)

  //compute (u.grad)u (convective derivative)
  FUNCTION(convectiveDerivative)

  // compute contravariant velocity from either cell-centered or face-centered input velocity
  FUNCTION(contravariantVelocity)

  FUNCTION(div)
  FUNCTION_COEFFICIENTS(divCoefficients)

  //returns cell-centered divergence given normal velocities
  FUNCTION(divNormal)

  // compute faceArea-weighted normal velocity from either cell-centered or 
  // face-centered input velocity (this is just an alias for contravariantVelocity)
  FUNCTION(normalVelocity)

  FUNCTION(grad)
  FUNCTION_COEFFICIENTS(gradCoefficients)

  FUNCTION(identity)
  FUNCTION_COEFFICIENTS(identityCoefficients)

  FUNCTION(laplacian)
  FUNCTION_COEFFICIENTS(laplacianCoefficients)

  FUNCTION(vorticity)

#undef FUNCTION  
#undef FUNCTION_COEFFICIENTS
  // ******* derivatives in non-standard  form  ***********

  //compute (u.grad)w (convective derivative of passive variable(s))
    virtual realMappedGridFunction convectiveDerivative (
                                                         const realMappedGridFunction &u, 
							 const realMappedGridFunction &w,
							 const Index & I1 = nullIndex,
							 const Index & I2 = nullIndex,
							 const Index & I3 = nullIndex
							 );
  virtual realMappedGridFunction convectiveDerivative (
                                                       const realMappedGridFunction &u,
						       const GridFunctionParameters & gfType, 
						       const realMappedGridFunction &w,
						       const Index & I1 = nullIndex,
						       const Index & I2 = nullIndex,
						       const Index & I3 = nullIndex
						       );


#define SCALAR_FUNCTION(type) \
  virtual realMappedGridFunction type(const realMappedGridFunction & u,  \
			              const realMappedGridFunction & s,  \
				      const Index & I1 = nullIndex,      \
				      const Index & I2 = nullIndex,      \
				      const Index & I3 = nullIndex,      \
				      const Index & I4 = nullIndex,      \
				      const Index & I5 = nullIndex,      \
				      const Index & I6 = nullIndex,      \
				      const Index & I7 = nullIndex,      \
				      const Index & I8 = nullIndex );    \
                                                                         \
  virtual realMappedGridFunction type(const realMappedGridFunction & u,  \
                                      const GridFunctionParameters & gfType,   \
			              const realMappedGridFunction & s, \
				      const Index & I1 = nullIndex,      \
				      const Index & I2 = nullIndex,      \
				      const Index & I3 = nullIndex,      \
				      const Index & I4 = nullIndex,      \
				      const Index & I5 = nullIndex,      \
				      const Index & I6 = nullIndex,      \
				      const Index & I7 = nullIndex,      \
				      const Index & I8 = nullIndex ); 

#define SCALAR_FUNCTION_COEFFICIENTS(type) \
  virtual realMappedGridFunction type(const realMappedGridFunction & s,  \
				      const Index & I1 = nullIndex,      \
				      const Index & I2 = nullIndex,      \
				      const Index & I3 = nullIndex,      \
				      const Index & I4 = nullIndex,      \
				      const Index & I5 = nullIndex,      \
				      const Index & I6 = nullIndex,      \
				      const Index & I7 = nullIndex,      \
				      const Index & I8 = nullIndex );    \
                                                                         \
  virtual realMappedGridFunction type(const GridFunctionParameters & gfType,   \
			              const realMappedGridFunction & s, \
                                      const Index & I1 = nullIndex,      \
				      const Index & I2 = nullIndex,      \
				      const Index & I3 = nullIndex,      \
				      const Index & I4 = nullIndex,      \
				      const Index & I5 = nullIndex,      \
				      const Index & I6 = nullIndex,      \
				      const Index & I7 = nullIndex,      \
				      const Index & I8 = nullIndex );

// div(s grad(u)), s=scalar field
SCALAR_FUNCTION(divScalarGrad)  
SCALAR_FUNCTION_COEFFICIENTS(divScalarGradCoefficients)  
// div( (1/s) grad(u)), s=scalar field
SCALAR_FUNCTION(divInverseScalarGrad)
SCALAR_FUNCTION_COEFFICIENTS(divInverseScalarGradCoefficients)  
// s grad(u)
SCALAR_FUNCTION(scalarGrad)
SCALAR_FUNCTION_COEFFICIENTS(scalarGradCoefficients)  

// div( sv u )
SCALAR_FUNCTION(divVectorScalar)
SCALAR_FUNCTION_COEFFICIENTS(divVectorScalarCoefficients)

#undef SCALAR_FUNCTION
#undef SCALAR_FUNCTION_COEFFICIENTS

  virtual realMappedGridFunction derivativeScalarDerivative(const realMappedGridFunction & u,  
							    const realMappedGridFunction & s,  
							    const int & direction1,
							    const int & direction2,
							    const Index & I1 = nullIndex,      
							    const Index & I2 = nullIndex,      
							    const Index & I3 = nullIndex,      
							    const Index & I4 = nullIndex,      
							    const Index & I5 = nullIndex,      
							    const Index & I6 = nullIndex,      
							    const Index & I7 = nullIndex,      
							    const Index & I8 = nullIndex );    
                                                                         
  virtual realMappedGridFunction derivativeScalarDerivative(const realMappedGridFunction & u,  
							    const GridFunctionParameters & gfType,   
							    const realMappedGridFunction & s, 
							    const int & direction1,
							    const int & direction2,
							    const Index & I1 = nullIndex,      
							    const Index & I2 = nullIndex,      
							    const Index & I3 = nullIndex,      
							    const Index & I4 = nullIndex,      
							    const Index & I5 = nullIndex,      
							    const Index & I6 = nullIndex,      
							    const Index & I7 = nullIndex,      
							    const Index & I8 = nullIndex ); 

  virtual realMappedGridFunction derivativeScalarDerivativeCoefficients(const realMappedGridFunction & s,  
									const int & direction1,
									const int & direction2,
									const Index & I1 = nullIndex,      
									const Index & I2 = nullIndex,      
									const Index & I3 = nullIndex,      
									const Index & I4 = nullIndex,      
									const Index & I5 = nullIndex,      
									const Index & I6 = nullIndex,      
									const Index & I7 = nullIndex,      
									const Index & I8 = nullIndex );    
                                                                         
  virtual realMappedGridFunction derivativeScalarDerivativeCoefficients(const GridFunctionParameters & gfType,   
									const realMappedGridFunction & s, 
									const int & direction1,
									const int & direction2,
									const Index & I1 = nullIndex,      
									const Index & I2 = nullIndex,      
									const Index & I3 = nullIndex,      
									const Index & I4 = nullIndex,      
									const Index & I5 = nullIndex,      
									const Index & I6 = nullIndex,      
									const Index & I7 = nullIndex,      
									const Index & I8 = nullIndex );


  //returns face-centered gradients
  virtual realMappedGridFunction FCgrad (const realMappedGridFunction & phi,		
                                         const int c0 = 0,
					 const int c1 = 0,
					 const int c2 = 0,
					 const int c3 = 0,
					 const int c4 = 0,
					 const Index & I1 = nullIndex,				
					 const Index & I2 = nullIndex,
					 const Index & I3 = nullIndex,
					 const Index & I4 = nullIndex,
					 const Index & I5 = nullIndex,
					 const Index & I6 = nullIndex, 
					 const Index & I7 = nullIndex, 
					 const Index & I8 = nullIndex 
                                        );

  virtual realMappedGridFunction FCgrad (const realMappedGridFunction & phi,		
                                         const GridFunctionParameters & gfType, 
					 const int c0 = 0,
					 const int c1 = 0,
					 const int c2 = 0,
					 const int c3 = 0,
					 const int c4 = 0,
					 const Index & I1 = nullIndex,				
					 const Index & I2 = nullIndex,
					 const Index & I3 = nullIndex,
					 const Index & I4 = nullIndex,
					 const Index & I5 = nullIndex,
					 const Index & I6 = nullIndex, 
					 const Index & I7 = nullIndex, 
					 const Index & I8 = nullIndex 
					 );


  // scalar times identityCoefficients
  virtual realMappedGridFunction scalarCoefficients(				
						    const realMappedGridFunction & s,
						    const Index & I1 = nullIndex,
						    const Index & I2 = nullIndex,
						    const Index & I3 = nullIndex,
						    const Index & I4 = nullIndex,
						    const Index & I5 = nullIndex, 
						    const Index & I6 = nullIndex, 
						    const Index & I7 = nullIndex, 
						    const Index & I8 = nullIndex
						    );


  // ********************************************************************
  // ------------- Here we define the Boundary Conditions ---------------
  // ********************************************************************

 public:
    
  enum boundaryConditionTypes   // get rid of this eventually **** Must match BCTypes ****
  {
    dirichlet=BCTypes::dirichlet,
    neumann=BCTypes::neumann,
    extrapolate=BCTypes::extrapolate,
    normalComponent=BCTypes::normalComponent,
    mixed=BCTypes::mixed,
    generalMixedDerivative=BCTypes::generalMixedDerivative,
    normalDerivativeOfNormalComponent=BCTypes::normalDerivativeOfNormalComponent,
    normalDerivativeOfADotU=BCTypes::normalDerivativeOfADotU,
    aDotU=BCTypes::aDotU,
    aDotGradU=BCTypes::aDotGradU,
    normalDotScalarGrad=BCTypes::normalDotScalarGrad,
    evenSymmetry=BCTypes::evenSymmetry,
    generalizedDivergence=BCTypes::generalizedDivergence,
    vectorSymmetry=BCTypes::vectorSymmetry,
    tangentialComponent0=BCTypes::tangentialComponent0,
    tangentialComponent1=BCTypes::tangentialComponent1,
    normalDerivativeOfTangentialComponent0=BCTypes::normalDerivativeOfTangentialComponent0,
    normalDerivativeOfTangentialComponent1=BCTypes::normalDerivativeOfTangentialComponent1,
    extrapolateInterpolationNeighbours=BCTypes::extrapolateInterpolationNeighbours,
    tangentialComponent=BCTypes::tangentialComponent,
    extrapolateNormalComponent=BCTypes::extrapolateNormalComponent,
    extrapolateTangentialComponent0=BCTypes::extrapolateTangentialComponent0,
    extrapolateTangentialComponent1=BCTypes::extrapolateTangentialComponent1,
    userDefinedBoundaryCondition0=BCTypes::userDefinedBoundaryCondition0,
    userDefinedBoundaryCondition1=BCTypes::userDefinedBoundaryCondition1,
    userDefinedBoundaryCondition2=BCTypes::userDefinedBoundaryCondition2,
    userDefinedBoundaryCondition3=BCTypes::userDefinedBoundaryCondition3,
    userDefinedBoundaryCondition4=BCTypes::userDefinedBoundaryCondition4,
    userDefinedBoundaryCondition5=BCTypes::userDefinedBoundaryCondition5,
    userDefinedBoundaryCondition6=BCTypes::userDefinedBoundaryCondition6,
    userDefinedBoundaryCondition7=BCTypes::userDefinedBoundaryCondition7,
    userDefinedBoundaryCondition8=BCTypes::userDefinedBoundaryCondition8,
    userDefinedBoundaryCondition9=BCTypes::userDefinedBoundaryCondition9,
    numberOfDifferentBoundaryConditionTypes   // counts number of entries in this list
  };

  virtual void applyBoundaryConditions(realMappedGridFunction & u, const real & time = 0.);
  // fill in coefficients for the boundary conditions
  virtual void assignBoundaryConditionCoefficients(realMappedGridFunction & coeff, const real & time = 0.);

  // +++++++++++++++++++ new BC interface: ++++++++++++++++++++++++++++++++++++
  virtual void applyBoundaryCondition(realMappedGridFunction & u, 
				      const Index & Components,
				      const BCTypes::BCNames & boundaryConditionType 
                                           = BCTypes::dirichlet,
                                      const int & boundaryCondition = allBoundaries,
				      const real & forcing = 0.,
				      const real & time = 0.,
				      const BoundaryConditionParameters & bcParameters 
				      = Overture::defaultBoundaryConditionParameters(),
                                      const int & grid=0 );
  

  virtual void applyBoundaryCondition(realMappedGridFunction & u, 
				      const Index & Components,
				      const BCTypes::BCNames & boundaryConditionType,
				      const int & boundaryCondition,
				      const RealArray & forcing,
				      const real & time = 0.,
				      const BoundaryConditionParameters & bcParameters 
				      = Overture::defaultBoundaryConditionParameters(),
                                      const int & grid=0 );
  

  virtual void applyBoundaryCondition(realMappedGridFunction & u, 
				      const Index & Components,
				      const BCTypes::BCNames & boundaryConditionType,
				      const int & boundaryCondition,
				      const RealArray & forcing,
				      realArray *forcinga[2][3],
				      const real & time = 0.,
				      const BoundaryConditionParameters & bcParameters 
				      = Overture::defaultBoundaryConditionParameters(),
                                      const int & grid=0 );
  

  virtual void applyBoundaryCondition(realMappedGridFunction & u, 
				      const Index & Components,
				      const BCTypes::BCNames & boundaryConditionType,
				      const int & boundaryCondition,
				      const realMappedGridFunction & forcing,
				      const real & time = 0.,
				      const BoundaryConditionParameters & bcParameters 
				      = Overture::defaultBoundaryConditionParameters(),
                                      const int & grid=0 );
  
#ifdef USE_PPP
  // this version takes a distributed array "forcing"
  void applyBoundaryCondition(realMappedGridFunction & u, 
                              const Index & Components,
                              const BCTypes::BCNames & boundaryConditionType,
                              const int & boundaryCondition,
                              const RealDistributedArray & forcing,
                              const real & time = 0.,
                              const BoundaryConditionParameters & bcParameters 
			      = Overture::defaultBoundaryConditionParameters(),
			      const int & grid=0 );
#endif

  // fix corners and periodic update:
  virtual void finishBoundaryConditions
    (realMappedGridFunction & u,
     const BoundaryConditionParameters & bcParameters= Overture::defaultBoundaryConditionParameters(),
     const Range & C0=nullRange);

  // the next routine is normally only called by finishBoundaryConditions
  virtual void fixBoundaryCorners
     (realMappedGridFunction & u,
      const BoundaryConditionParameters & bcParameters= Overture::defaultBoundaryConditionParameters(),
      const Range & C0=nullRange);

  // fill in the BC's for a coefficient grid function
  virtual void 
  applyBoundaryConditionCoefficients(
                                     realMappedGridFunction & coeff, 
				     const Index & Equations,
				     const Index & Components,
				     const BCTypes::BCNames & boundaryConditionType=BCTypes::dirichlet,
				     const int & boundaryCondition= allBoundaries,
				     const BoundaryConditionParameters & bcParameters 
				     = Overture::defaultBoundaryConditionParameters(),
                                      const int & grid=0 );


  virtual void useConservativeApproximations(bool trueOrFalse = TRUE );
  bool usingConservativeApproximations() const;

  void setAveragingType(const AveragingType & type );
  AveragingType getAveragingType() const;

  realArray harmonic(const realArray & a, const realArray & b ); // harmonic average

  // here are some parameters -- these are public for now ?? ------
  int orderOfAccuracy;
  int stencilSize;                             //
  int numberOfComponentsForCoefficients;       // for dimensioning coefficient matrices
  int twilightZoneFlow;                        // 0, 1 or 2.
  OGFunction *twilightZoneFlowFunction;

  protected:
  // These next functions are called by finishBoundaryConditions

  virtual void setCornerCoefficients(realMappedGridFunction & coeff,
                                     const int n, 
				     const Index & I1, 
				     const Index & I2, 
				     const Index & I3,
				     int side1,
                                     int side2,
                                     int side3,
                                     const BoundaryConditionParameters & bcParameters );

  virtual void setExtrapolationCoefficients(realMappedGridFunction & coeff,
				    const int n, 
				    const Index & I1, 
				    const Index & I2, 
				    const Index & I3,
				    const int order);

  virtual void setSymmetryCoefficients(realMappedGridFunction & coeff,
                                       const int n, 
				       const Index & I1, 
				       const Index & I2, 
				       const Index & I3,
				       const int option = 0);

  virtual void setPeriodicCoefficients(realMappedGridFunction & coeff );

  // routine for setting up arrays for extrapolating neighbours to interpolation points
  virtual void findInterpolationNeighbours();

  // These next arrays are used for BC=extrapolateInterpolationNeighbours 
  // numberOfInterpolationNeighbours[axis] : holds the number of points to extrapolate 
  // ipn[axis](m,0:2) : holds indicies of points to extrapolate 
  // ipd[axis](m) : holds direction of extrapolation (+1/-1)
  int extrapolateInterpolationNeighboursIsInitialized;
  int numberOfInterpolationNeighbours[3];
  IntegerArray ipn[3];  
  IntegerArray ipd[3]; 

  int numberOfInterpolationNeighboursNew;
  intArray extrapolateInterpolationNeighbourPoints, extrapolateInterpolationNeighboursDirection;

  bool conservative;  

  AveragingType averagingType;

};






#endif
