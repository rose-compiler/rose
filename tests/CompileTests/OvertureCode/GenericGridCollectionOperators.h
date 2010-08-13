/*  -*-Mode: c++; -*-  */

#ifndef GENERIC_GRID_COLLECTION_OPERATORS
#define GENERIC_GRID_COLLECTION_OPERATORS

// The file GenericCompositeGridOperators.h is created from GenericGridCollectionOperators.h by the
// perl script gf.p

#undef COMPOSITE_GRID_OPERATORS
// The next line is uncommented in GenericCompositeGridOperators.h
// define COMPOSITE_GRID_OPERATORS


#include "GenericMappedGridOperators.h"
#include "ListOfGenericMappedGridOperators.h"
#include "BoundaryConditionParameters.h"
#include "GenericCollectionOperators.h"

// extern BoundaryConditionParameters Overture::defaultBoundaryConditionParameters();

class GridFunctionParameters;


//-----------------------------------------------------------------------------------
//  
//  Define differential and Boundary operators associated with GridCollections,
//  GridCollectionFunctions (and CompositeGrid/CompositeGridFunctions)
//
//-----------------------------------------------------------------------------------

class GenericGridCollectionOperators : public GenericCollectionOperators
{

 public:

  enum         
  {
    forAll=realMappedGridFunction::forAll,
    undefinedValue=realMappedGridFunction::undefinedValue
  };

  
  GridCollection gridCollection;           // operators are defined for this GridCollection

  // ********************************************************************
  // **************** Miscellaneous Functions **************************
  // ********************************************************************

  // default constructor
  GenericGridCollectionOperators();
  // contructor taking a GridCollection
  GenericGridCollectionOperators( GridCollection & mg );                  
  // copy constructor
  GenericGridCollectionOperators( const GenericGridCollectionOperators & mgo );  

  GenericGridCollectionOperators( GenericMappedGridOperators & mgo );  
  GenericGridCollectionOperators( GridCollection & mg, GenericMappedGridOperators & mgo );                  
  // create a new object of this class
  virtual GenericGridCollectionOperators* virtualConstructor();        
  
  virtual ~GenericGridCollectionOperators();

  virtual GenericGridCollectionOperators & operator= ( const GenericGridCollectionOperators & mgo );

  // here is the operator for a GenericMappedGrid
  GenericMappedGridOperators & operator[]( const int grid ) const;

  // return size of this object  
  virtual real sizeOf(FILE *file = NULL ) const;

  // supply a new grid to use
  virtual void updateToMatchGrid( GridCollection & g );         
  // Use some new operators
  virtual void updateToMatchOperators(GenericMappedGridOperators & op);
  
  // get from a database file
  virtual int get( const GenericDataBase & dir, const aString & name);  
  // put to a database file
  virtual int put( GenericDataBase & dir, const aString & name) const;  

  // set the order of accuracy
  virtual void setOrderOfAccuracy( const int & orderOfAccuracy );
  
  // Indicate the stencil size for functions returning coefficients
  virtual void setStencilSize(const int stencilSize);

  // Indicate the number of components (system size) for functions returning coefficients
  virtual void setNumberOfComponentsForCoefficients(const int number);

  // Indicate if twilight-zone forcing should be added to boundary conditions  
  void setTwilightZoneFlow( const int & twilightZoneFlow );

  // Supply a twilight-zone forcing to use for boundary conditions  
  void setTwilightZoneFlowFunction( OGFunction & twilightZoneFlowFunction );

  virtual void useConservativeApproximations(bool trueOrFalse = TRUE );
  void setAveragingType(const GenericMappedGridOperators::AveragingType & type );

  // ************************************************
  // ***** DIFFERENTIATION CLASS FUNCTIONS **********
  // ************************************************


// Macro to define a typical function call
#define FUNCTION(type) \
  virtual realGridCollectionFunction type(const realGridCollectionFunction & u,  \
				          const Index & C1 = nullIndex,      \
				          const Index & C2 = nullIndex,      \
				          const Index & C3 = nullIndex,      \
				          const Index & C4 = nullIndex,      \
					  const Index & C5 = nullIndex ); \
  virtual realGridCollectionFunction type(const realGridCollectionFunction & u,  \
                                          const GridFunctionParameters & gfType, \
				          const Index & C1 = nullIndex,      \
				          const Index & C2 = nullIndex,      \
				          const Index & C3 = nullIndex,      \
				          const Index & C4 = nullIndex,      \
					  const Index & C5 = nullIndex ); 
  

#define FUNCTION_COEFFICIENTS(type) \
  virtual realGridCollectionFunction type(const Index & C0 = nullIndex,      \
				          const Index & C1 = nullIndex,      \
				          const Index & C2 = nullIndex,      \
				          const Index & C3 = nullIndex );    \
  \
  virtual realGridCollectionFunction type(const GridFunctionParameters & gfType, \
                                          const Index & C0 = nullIndex,      \
				          const Index & C1 = nullIndex,      \
				          const Index & C2 = nullIndex,      \
				          const Index & C3 = nullIndex );

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
  virtual realGridCollectionFunction convectiveDerivative (
                                               const realGridCollectionFunction &u, 
					       const realGridCollectionFunction &w,
					       const Index & C1 = nullIndex,
					       const Index & C2 = nullIndex,
					       const Index & C3 = nullIndex
							   );
  virtual realGridCollectionFunction convectiveDerivative (
                                                           const realGridCollectionFunction &u, 
							   const GridFunctionParameters & gfType,
							   const realGridCollectionFunction &w,
							   const Index & C1 = nullIndex,
							   const Index & C2 = nullIndex,
							   const Index & C3 = nullIndex
							   );

// Macro to define a typical function call
#define SCALAR_FUNCTION(type) \
  virtual realGridCollectionFunction type(const realGridCollectionFunction & u,  \
				          const realGridCollectionFunction & s, \
				          const Index & C1 = nullIndex,      \
				          const Index & C2 = nullIndex,      \
				          const Index & C3 = nullIndex,      \
					  const Index & C4 = nullIndex ); \
  virtual realGridCollectionFunction type(const realGridCollectionFunction & u,  \
                                          const GridFunctionParameters & gfType, \
				          const realGridCollectionFunction & s, \
				          const Index & C1 = nullIndex,      \
				          const Index & C2 = nullIndex,      \
				          const Index & C3 = nullIndex,      \
					  const Index & C4 = nullIndex ); 
  

#define SCALAR_FUNCTION_COEFFICIENTS(type) \
  virtual realGridCollectionFunction type(const realGridCollectionFunction & s, \
                                          const Index & C0 = nullIndex,      \
				          const Index & C1 = nullIndex,      \
				          const Index & C2 = nullIndex,      \
				          const Index & C3 = nullIndex );    \
  \
  virtual realGridCollectionFunction type(const GridFunctionParameters & gfType, \
				          const realGridCollectionFunction & s, \
                                          const Index & C0 = nullIndex,      \
				          const Index & C1 = nullIndex,      \
				          const Index & C2 = nullIndex,      \
				          const Index & C3 = nullIndex );


  // div(s grad(u)), s=scalar field
  SCALAR_FUNCTION(divScalarGrad)
  SCALAR_FUNCTION_COEFFICIENTS(divScalarGradCoefficients)

  SCALAR_FUNCTION(divInverseScalarGrad)
  SCALAR_FUNCTION_COEFFICIENTS(divInverseScalarGradCoefficients)

  SCALAR_FUNCTION(scalarGrad)
  SCALAR_FUNCTION_COEFFICIENTS(scalarGradCoefficients)

  SCALAR_FUNCTION(divVectorScalar)
  SCALAR_FUNCTION_COEFFICIENTS(divVectorScalarCoefficients)


#undef SCALAR_FUNCTION
#undef SCALAR_FUNCTION_COEFFICIENTS


  virtual realGridCollectionFunction derivativeScalarDerivative(const realGridCollectionFunction & u,  
				          const realGridCollectionFunction & s, 
					  const int & direction1,
					  const int & direction2,
				          const Index & C1 = nullIndex,      
				          const Index & C2 = nullIndex,      
				          const Index & C3 = nullIndex,      
					  const Index & C4 = nullIndex ); 
  virtual realGridCollectionFunction derivativeScalarDerivative(const realGridCollectionFunction & u,  
                                          const GridFunctionParameters & gfType, 
				          const realGridCollectionFunction & s, 
					  const int & direction1,
					  const int & direction2,
				          const Index & C1 = nullIndex,      
				          const Index & C2 = nullIndex,      
				          const Index & C3 = nullIndex,      
					  const Index & C4 = nullIndex ); 
  

  virtual realGridCollectionFunction derivativeScalarDerivativeCoefficients(const realGridCollectionFunction & s, 
					  const int & direction1,
					  const int & direction2,
                                          const Index & C0 = nullIndex,      
				          const Index & C1 = nullIndex,      
				          const Index & C2 = nullIndex,      
				          const Index & C3 = nullIndex );    
  
  virtual realGridCollectionFunction derivativeScalarDerivativeCoefficients(const GridFunctionParameters & gfType, 
				          const realGridCollectionFunction & s, 
					  const int & direction1,
					  const int & direction2,
                                          const Index & C0 = nullIndex,      
				          const Index & C1 = nullIndex,      
				          const Index & C2 = nullIndex,      
				          const Index & C3 = nullIndex );


  //returns face-centered gradients
  virtual realGridCollectionFunction FCgrad (const realGridCollectionFunction & phi,		
                                             const int c0 = 0,
					     const int c1 = 0,
					     const int c2 = 0,
					     const int c3 = 0,
					     const int c4 = 0
					     );
  virtual realGridCollectionFunction FCgrad (const realGridCollectionFunction & phi,		
                                             const GridFunctionParameters & gfType,
					     const int c0 = 0,
					     const int c1 = 0,
					     const int c2 = 0,
					     const int c3 = 0,
					     const int c4 = 0
					     );



  // ********************************************************************
  // ------------- Here we define the Boundary Conditions ---------------
  // ********************************************************************

 public:
    
  virtual void applyBoundaryConditions(realGridCollectionFunction & u, 
                                       const real & time = 0.,
				       const int & grid =forAll);
  // fill in coefficients for the boundary conditions
  virtual void assignBoundaryConditionCoefficients(realGridCollectionFunction & coeff, 
                                                   const real & time = 0.,
						   const int & grid =forAll);


  // new BC interface:
  virtual void applyBoundaryCondition(realGridCollectionFunction & u, 
                              const Index & Components,
                              const BCTypes::BCNames & boundaryConditionType=BCTypes::dirichlet,
                              const int & boundaryCondition = BCTypes::allBoundaries,
                              const real & forcing = 0.,
                              const real & time = 0.,
                              const BoundaryConditionParameters & bcParameters 
                                          = Overture::defaultBoundaryConditionParameters());
  

  virtual void applyBoundaryCondition(realGridCollectionFunction & u, 
                              const Index & Components,
                              const BCTypes::BCNames & boundaryConditionType,
                              const int & boundaryCondition,
                              const RealArray & forcing,
                              const real & time = 0.,
                              const BoundaryConditionParameters & bcParameters 
                                          = Overture::defaultBoundaryConditionParameters());

  virtual void applyBoundaryCondition(realGridCollectionFunction & u, 
                              const Index & Components,
                              const BCTypes::BCNames & boundaryConditionType,
                              const int & boundaryCondition,
                              const realGridCollectionFunction & forcing,
                              const real & time = 0.,
                              const BoundaryConditionParameters & bcParameters 
                                          = Overture::defaultBoundaryConditionParameters());

#ifdef USE_PPP
  virtual void applyBoundaryCondition(realGridCollectionFunction & u, 
                              const Index & Components,
                              const BCTypes::BCNames & boundaryConditionType,
                              const int & boundaryCondition,
                              const RealDistributedArray & forcing,
                              const real & time = 0.,
                              const BoundaryConditionParameters & bcParameters 
                                          = Overture::defaultBoundaryConditionParameters());
#endif
  // fix corners and periodic update:
  virtual void finishBoundaryConditions
    (realGridCollectionFunction & u,
     const BoundaryConditionParameters & bcParameters= Overture::defaultBoundaryConditionParameters(),
     const Range & C0=nullRange);

  // fill in the BC's for a coefficient grid function
  virtual void applyBoundaryConditionCoefficients(realGridCollectionFunction & coeff, 
				          const Index & Equations,
					  const Index & Components,
					  const BCTypes::BCNames & boundaryConditionType=BCTypes::dirichlet,
					  const int & boundaryCondition= BCTypes::allBoundaries,
					  const BoundaryConditionParameters & bcParameters 
                                          = Overture::defaultBoundaryConditionParameters());
  protected:  

// this next function is only found in GenericCompositeGridOperators:
#ifdef COMPOSITE_GRID_OPERATORS
  void getInterpolationCoefficients(realCompositeGridFunction & coeff );
#endif

#undef COMPOSITE_GRID_OPERATORS

  int stencilSize;

  Index I1,I2,I3;
  // realGridCollectionFunction result;                       // derivative is returned in this grid function
  ListOfGenericMappedGridOperators mappedGridOperators;
  GenericMappedGridOperators *mappedGridOperatorsPointer;  // pointer to an MappedGridOperators object
                                                           // (this may point to a derived class object)
  bool twilightZoneFlow;
  OGFunction *twilightZoneFlowFunction;

  void setup();

};






#endif
