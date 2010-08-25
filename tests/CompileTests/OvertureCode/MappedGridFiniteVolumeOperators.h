/*  -*-Mode: c++; -*-  */
#ifndef REAL_MAPPED_GRID_FINITE_VOLUME_OPERATORS
#define REAL_MAPPED_GRID_FINITE_VOLUME_OPERATORS

#include "davidsReal.h"
#include <assert.h>
#include <CompositeGrid.h>
#include <CompositeGridFunction.h>
#include <Oges.h>
#include <NameList.h>
#include <Interpolant.h>
#include <OGgetIndex.h>
#include <PlotStuff.h>
#include "axisDefs.h"
#include "loops.h"
#include "Display.h"
// *wdh*
#include "GenericMappedGridOperators.h"

//------------------------------------------------------------------------------
//
//	class MappedGridFiniteVolumeOperators
//      NEW version derived from GenericMappedGridOperators
//
//	The purpose of this class is to define operators on 
//	mapped grids for finite volume codes.
//	D. L. Brown 950110
//
//------------------------------------------------------------------------------

class MappedGridFiniteVolumeOperators : public GenericMappedGridOperators
{
  public :

	// ========================================
	// PUBLIC DATA
	// ========================================

  static bool debug;

  static const Index nullIndex;
  static const Range all;
  int defaultPositionOfComponent;			//this will be used when constructing all realMappedGridFunction's
  int numberOfDimensions;

	// ========================================
	// ENUMS
	// ========================================

  enum differentiationTypes		// these aren't used for anything (yet) so don't get too excited
  {
    cellCenteredDivergence,
    cellCenteredGradient,
    faceCenteredGradient,
    numberOfDifferentDifferentiationTypes  // counts number of entries
  };

  enum boundaryConditionTypes
  {
    periodic = -1,
    dirichlet ,			
    neumann,
    extrapolate,
    normalComponent,
    numberOfDifferentBoundaryConditionTypes  // counts number of entries; explicitly set for debugging
  };

  enum LaplacianType
  {
    constCoeff,
    inverseScalarVariableCoefficients,
    variableCoefficients
  };
    

  enum
  {
    undefinedValue=-999,
    defaultValue=-998,
    forAll=-997
  };

  enum functionNames
  {
    setBoundaryConditionCheck,
    setBoundaryConditionValueCheck,
    applyBoundaryConditionsCheck,
    ApplyBoundaryConditionsCheck,
    numberOfFunctionNames
  };

	// ========================================
	// CONSTRUCTORS
	// ========================================

  MappedGridFiniteVolumeOperators ();                                               // default constructor
  MappedGridFiniteVolumeOperators (MappedGrid & mg);                                // make from this MappedGrid
  MappedGridFiniteVolumeOperators (const MappedGridFiniteVolumeOperators & mgfvo);  // copy constructor NEW
  virtual GenericMappedGridOperators* virtualConstructor() const;                    // create a new object of this class NEW

  virtual ~MappedGridFiniteVolumeOperators ();

  virtual MappedGridFiniteVolumeOperators & operator= (const MappedGridFiniteVolumeOperators &fvo);
  virtual GenericMappedGridOperators & operator= ( const GenericMappedGridOperators & mgo );

  virtual void updateToMatchGrid (MappedGrid &mg);

	// ========================================
	// GENERAL CLASS FUNCTIONS
	// ========================================

  void setIsVolumeScaled (const bool trueOrFalse);	
  bool getIsVolumeScaled ();	

  void setUseCMPGRDGeometryArrays (const bool trueOrFalse);
  bool getUseCMPGRDGeometryArrays ();

// ***this shouldn't be here
//  void setTwilightZoneFlow (const bool &TwilightZoneFlow0);		// indicate that forcing is to be used in BCs
//  void setTwilightZoneFlowFunction (OGFunction & TwilightZoneFlowFunction0);
									// indicate which forcing is to be used

	// ========================================
	// DIFFERENTIATION CLASS FUNCTIONS
	// ========================================



#define FUNCTION(type) \
  virtual realMappedGridFunction type(const realMappedGridFunction & u,  \
				      const Index & I1 = nullIndex,      \
				      const Index & I2 = nullIndex,      \
				      const Index & I3 = nullIndex,      \
				      const Index & I4 = nullIndex,      \
				      const Index & I5 = nullIndex,      \
				      const Index & I6 = nullIndex,      \
				      const Index & I7 = nullIndex,      \
				      const Index & I8 = nullIndex ) 

#define TYPED_FUNCTION(type) \
  virtual realMappedGridFunction type(const realMappedGridFunction & u, \
				      const GridFunctionParameters & gft, \
				      const Index & I1 = nullIndex,      \
				      const Index & I2 = nullIndex,      \
				      const Index & I3 = nullIndex,      \
				      const Index & I4 = nullIndex,      \
				      const Index & I5 = nullIndex,      \
				      const Index & I6 = nullIndex,      \
				      const Index & I7 = nullIndex,      \
				      const Index & I8 = nullIndex ) 

#define FUNCTION_COEFFICIENTS(type) \
  virtual realMappedGridFunction type(const Index & I1 = nullIndex,      \
				      const Index & I2 = nullIndex,      \
				      const Index & I3 = nullIndex,      \
				      const Index & I4 = nullIndex,      \
				      const Index & I5 = nullIndex,      \
				      const Index & I6 = nullIndex,      \
				      const Index & I7 = nullIndex,      \
				      const Index & I8 = nullIndex ) 



#define TYPED_FUNCTION_COEFFICIENTS(type) \
    virtual realMappedGridFunction type(const GridFunctionParameters & gft, \
					const Index & I1 = nullIndex,      \
					const Index & I2 = nullIndex,      \
					const Index & I3 = nullIndex,      \
					const Index & I4 = nullIndex,      \
					const Index & I5 = nullIndex,      \
					const Index & I6 = nullIndex,      \
					const Index & I7 = nullIndex,      \
					const Index & I8 = nullIndex ) 

  FUNCTION (contravariantVelocity);
  FUNCTION (normalVelocity);
  FUNCTION (cellsToFaces);
  FUNCTION (convectiveDerivative);
  FUNCTION (vorticity);

  FUNCTION (div);
  FUNCTION (divNormal);
  FUNCTION (grad);
  FUNCTION (laplacian);
  FUNCTION (x);
  FUNCTION (y);
  FUNCTION (z);

  TYPED_FUNCTION (contravariantVelocity);
  TYPED_FUNCTION (normalVelocity);
  TYPED_FUNCTION (cellsToFaces);
  TYPED_FUNCTION (convectiveDerivative);
  TYPED_FUNCTION (vorticity);
  TYPED_FUNCTION (div);
  TYPED_FUNCTION (divNormal);
  TYPED_FUNCTION (grad);
  TYPED_FUNCTION (laplacian);
  TYPED_FUNCTION (x);  
  TYPED_FUNCTION (y);
  TYPED_FUNCTION (z);

  FUNCTION_COEFFICIENTS (laplacianCoefficients);
  FUNCTION_COEFFICIENTS (identityCoefficients);
  FUNCTION_COEFFICIENTS (xCoefficients);
  FUNCTION_COEFFICIENTS (yCoefficients);
  FUNCTION_COEFFICIENTS (zCoefficients);

  TYPED_FUNCTION_COEFFICIENTS (laplacianCoefficients);
  TYPED_FUNCTION_COEFFICIENTS (identityCoefficients);
  TYPED_FUNCTION_COEFFICIENTS (xCoefficients);
  TYPED_FUNCTION_COEFFICIENTS (yCoefficients);
  TYPED_FUNCTION_COEFFICIENTS (zCoefficients);

#undef FUNCTION  
#undef TYPED_FUNCTION
#undef FUNCTION_COEFFICIENTS 
#undef TYPED_FUNCTION_COEFFICIENTS  
//
//  ... functions with non-standard arguments      
//

  virtual REALMappedGridFunction identityOperator (
                                                   const Index & I1,
						   const Index & I2,
						   const Index & I3);
  

  virtual REALMappedGridFunction faceAverage (   
			const REALMappedGridFunction &u,			// average cell centered quantities to faces
			const int axis1,					// or face-centered quantities to centers
			const int axis2,						
			const int c0=0,
			const int c1=0,
			const int c2=0,
			const int c3=0,
			const int c4=0,
			const Index & I1 = nullIndex ,				// cell-to-face is a "backward" average;
			const Index & I2 = nullIndex,
			const Index & I3 = nullIndex
			);							// face-to-cell is a "forward" average;

  virtual REALMappedGridFunction faceAverage (   
					      const REALMappedGridFunction &u,			// average cell centered quantities to faces
					      const GridFunctionParameters & gft,
					      const int axis1,					// or face-centered quantities to centers
					      const int axis2,						
					      const int c0=0,
					      const int c1=0,
					      const int c2=0,
					      const int c3=0,
					      const int c4=0,
					      const Index & I1 = nullIndex ,				// cell-to-face is a "backward" average;
					      const Index & I2 = nullIndex,
					      const Index & I3 = nullIndex
					      );							// face-to-cell is a "forward" average;

  virtual REALMappedGridFunction average (						// average a particular component
					  const REALMappedGridFunction &u,
					  const int  axis=allAxes,
					  const int  c0=0,
					  const int  c1=0,
					  const int  c2=0,
					  const int  c3=0,
					  const int  c4=0,
					  const Index & I1 = nullIndex,
					  const Index & I2 = nullIndex,
					  const Index & I3 = nullIndex
					  );

  virtual REALMappedGridFunction average (						// average a particular component
					  const REALMappedGridFunction &u,
					  const GridFunctionParameters & gft,
					  const int  axis=allAxes,
					  const int  c0=0,
					  const int  c1=0,
					  const int  c2=0,
					  const int  c3=0,
					  const int  c4=0,
					  const Index & I1 = nullIndex,
					  const Index & I2 = nullIndex,
					  const Index & I3 = nullIndex
					  );

  virtual REALMappedGridFunction convectiveDerivative (
                                                       const REALMappedGridFunction &u,  //compute (u.grad)w (convective derivative of passive variable(s))
						       const REALMappedGridFunction &w,
						       const Index & I1 = nullIndex,
						       const Index & I2 = nullIndex,
						       const Index & I3 = nullIndex
						       );
  virtual REALMappedGridFunction convectiveDerivative (
                                                       const REALMappedGridFunction &u,  //compute (u.grad)w (convective derivative of passive variable(s))
						       const GridFunctionParameters & gft,
						       const REALMappedGridFunction &w,
						       const Index & I1 = nullIndex,
						       const Index & I2 = nullIndex,
						       const Index & I3 = nullIndex
						       );

  virtual REALMappedGridFunction difference (
                                             const REALMappedGridFunction &u,
					     const int axis,
					     const int c0=0,
					     const int c1=0,
					     const int c2=0,
					     const int c3=0,
					     const int c4=0,
					     const Index &I1=nullIndex,
					     const Index &I2=nullIndex,
					     const Index &I3=nullIndex
					     );
  virtual REALMappedGridFunction difference (
                                             const REALMappedGridFunction &u,
					     const GridFunctionParameters & gft,
					     const int axis,
					     const int c0=0,
					     const int c1=0,
					     const int c2=0,
					     const int c3=0,
					     const int c4=0,
					     const Index &I1=nullIndex,
					     const Index &I2=nullIndex,
					     const Index &I3=nullIndex
					     );

  virtual REALMappedGridFunction dZero (
					const REALMappedGridFunction &u,
					const int axis,
					const int c0=0,
					const int c1=0,
					const int c2=0,
					const int c3=0,
					const int c4=0,
					const Index &I1=nullIndex,
					const Index &I2=nullIndex,
					const Index &I3=nullIndex
					);
  virtual REALMappedGridFunction dZero (
					const REALMappedGridFunction &u,
					const GridFunctionParameters & gft,
					const int axis,
					const int c0=0,
					const int c1=0,
					const int c2=0,
					const int c3=0,
					const int c4=0,
					const Index &I1=nullIndex,
					const Index &I2=nullIndex,
					const Index &I3=nullIndex
					);


  virtual REALMappedGridFunction div (
				      const REALMappedGridFunction & u, 		//plot-debug version of div
				      PlotStuff * ps,
				      PlotStuffParameters * psp,
				      bool & PLOT_ON,
				      const Index & I1 = nullIndex,				
				      const Index & I2 = nullIndex,
				      const Index & I3 = nullIndex,
				      const Index & I4 = nullIndex,
				      const Index & I5 = nullIndex,
				      const Index & I6 = nullIndex,
				      const Index & I7 = nullIndex,
				      const Index & I8 = nullIndex
				      );
  virtual REALMappedGridFunction divNormal (
					    const REALMappedGridFunction & uBar, 		//plot-debug version of divNormal
					    PlotStuff * ps,
					    PlotStuffParameters * psp,
					    bool & PLOT_ON,
					    const Index & I1 = nullIndex,				
					    const Index & I2 = nullIndex,
					    const Index & I3 = nullIndex,
					    const Index & I4 = nullIndex,
					    const Index & I5 = nullIndex,
					    const Index & I6 = nullIndex,
					    const Index & I7 = nullIndex,
					    const Index & I8 = nullIndex);


  virtual REALMappedGridFunction FCgrad (const REALMappedGridFunction & phi,	//
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
  virtual REALMappedGridFunction FCgrad (const REALMappedGridFunction & phi,
                                         const GridFunctionParameters & gft,    //
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

/* Old version v6
  virtual REALMappedGridFunction FCgrad (const REALMappedGridFunction & phi,	//
                                         const c0 = 0,
					 const c1 = 0,
					 const c2 = 0,
					 const c3 = 0,
					 const c4 = 0,
					 const Index & I1 = nullIndex,				
					 const Index & I2 = nullIndex,
					 const Index & I3 = nullIndex
					 );

  virtual REALMappedGridFunction FCgrad (const REALMappedGridFunction & phi,
                                         const GridFunctionParameters & gft,    //
					 const c0 = 0,
					 const c1 = 0,
					 const c2 = 0,
					 const c3 = 0,
					 const c4 = 0,
					 const Index & I1 = nullIndex,				
					 const Index & I2 = nullIndex,
					 const Index & I3 = nullIndex
					 );
*/

  virtual REALMappedGridFunction divScalarGrad(				// div(s.grad(u))
                                               const REALMappedGridFunction & u,
					       const REALMappedGridFunction & s,
					       const Index & I1 = nullIndex,
					       const Index & I2 = nullIndex,
					       const Index & I3 = nullIndex,
					       const Index & N  = nullIndex,
					       const Index & I5 = nullIndex,
					       const Index & I6 = nullIndex,
					       const Index & I7 = nullIndex,
					       const Index & I8 = nullIndex
					       );
  virtual REALMappedGridFunction divScalarGrad(				// div(s.grad(u))
                                               const REALMappedGridFunction & u,
					       const GridFunctionParameters & gft,
					       const REALMappedGridFunction & s,
					       const Index & I1 = nullIndex,
					       const Index & I2 = nullIndex,
					       const Index & I3 = nullIndex,
					       const Index & N  = nullIndex,
					       const Index & I5 = nullIndex,
					       const Index & I6 = nullIndex,
					       const Index & I7 = nullIndex,
					       const Index & I8 = nullIndex
					       );

  virtual REALMappedGridFunction divInverseScalarGrad(				// div((1/s).grad(u))
						      const REALMappedGridFunction & u,
						      const REALMappedGridFunction & s,
						      const Index & I1 = nullIndex,
						      const Index & I2 = nullIndex,
						      const Index & I3 = nullIndex,
						      const Index & N  = nullIndex,
						      const Index & I5 = nullIndex,
						      const Index & I6 = nullIndex,
						      const Index & I7 = nullIndex,
						      const Index & I8 = nullIndex 
						      );
  virtual REALMappedGridFunction divInverseScalarGrad(				// div((1/s).grad(u))
						      const REALMappedGridFunction & u,
						      const GridFunctionParameters & gft,
						      const REALMappedGridFunction & s,
						      const Index & I1 = nullIndex,
						      const Index & I2 = nullIndex,
						      const Index & I3 = nullIndex,
						      const Index & N  = nullIndex,
						      const Index & I5 = nullIndex,
						      const Index & I6 = nullIndex,
						      const Index & I7 = nullIndex,
						      const Index & I8 = nullIndex 
						      );


  virtual REALMappedGridFunction divScalarGradCoefficients(				// div(s.grad( )) operator
                                                           const REALMappedGridFunction & s,
							   const Index & I1 = nullIndex,
							   const Index & I2 = nullIndex,
							   const Index & I3 = nullIndex,
							   const Index & N  = nullIndex,
							   const Index & I5 = nullIndex,
							   const Index & I6 = nullIndex,
							   const Index & I7 = nullIndex,
							   const Index & I8 = nullIndex);

  virtual REALMappedGridFunction divScalarGradCoefficients(const GridFunctionParameters & gft, // div(s.grad( )) operator
                                                           const REALMappedGridFunction & s,
							   const Index & I1 = nullIndex,
							   const Index & I2 = nullIndex,
							   const Index & I3 = nullIndex,
							   const Index & N  = nullIndex,
							   const Index & I5 = nullIndex,
							   const Index & I6 = nullIndex,
							   const Index & I7 = nullIndex,
							   const Index & I8 = nullIndex);
  

  virtual REALMappedGridFunction divInverseScalarGradCoefficients(				// div((1/s).grad( )) operator
								  const REALMappedGridFunction & s,
								  const Index & I1 = nullIndex,
								  const Index & I2 = nullIndex,
								  const Index & I3 = nullIndex,
								  const Index & N  = nullIndex,
								  const Index & I5 = nullIndex,
								  const Index & I6 = nullIndex,
								  const Index & I7 = nullIndex,
								  const Index & I8 = nullIndex
								  );
  virtual REALMappedGridFunction divInverseScalarGradCoefficients(				// div((1/s).grad( )) operator
								  const GridFunctionParameters & gft,
								  const REALMappedGridFunction & s,
								  const Index & I1 = nullIndex,
								  const Index & I2 = nullIndex,
								  const Index & I3 = nullIndex,
								  const Index & N  = nullIndex,
								  const Index & I5 = nullIndex,
								  const Index & I6 = nullIndex,
								  const Index & I7 = nullIndex,
								  const Index & I8 = nullIndex
								  );

  //
// ... Boundary condition Operators
//

  // how many BCs on each side
  void setNumberOfBoundaryConditions(
			const int & numberOfBoundaryConditions,
			const int & side = forAll,
			const int & axis = forAll
		      );
  
  // associate a BC with a given boundary
  void setBoundaryCondition(
			const int & boundaryConditionNumber,
			const int & side,
			const int & axis,
			const boundaryConditionTypes & boundaryConditionType,
			const int & component = 0
		      );

  // associate a BC with a given boundary
  // use this fn when a boundary condition involves more than one component such
  // as setting the normal component of a vector
  void setBoundaryCondition (
			const int & boundaryConditionNumber,
			const int & side,
			const int & axis,
			const boundaryConditionTypes & boundaryConditionType,
			const intArray & component );

  // set all BCs to same type
  void setBoundaryCondition (
			const boundaryConditionTypes & boundaryCondition
			);

  // supply a boundary value for a boundary condition (or turn off this option)

  void setBoundaryConditionValue (
			const real & value,
			const int & component,
			const int & index,
			const int & side=forAll,
			const int & axis=forAll,
			const bool & trueOrFalse=TRUE);

  void setBoundaryConditionRightHandSide (const REALMappedGridFunction & boundaryConditionRightHandSide);

  void applyBoundaryConditions(
			realMappedGridFunction & u,
			const real & time = 0.);

  void applyBoundaryConditionsToCoefficients(                                 //replaces ApplyBoundaryConditions
                                             REALMappedGridFunction & coeff,
					     const real & time = 0.);
  

  void ApplyBoundaryConditions(realMappedGridFunction & coeff, 
			const real & time = 0.); 		// fill in coefficients

  void applyRightHandSideBoundaryConditions(
			realMappedGridFunction & rightHandSide,
		        const real & time = 0.
					    );                  // apply BCs to RHS for elliptic solve
  

// This has been moved to the base class in Overture.v1
//  void fixBoundaryCorners(realMappedGridFunction & u);

  // new BC interface:
  void applyBoundaryCondition(realMappedGridFunction & u, 
                              const Index & Components,
			      const BCTypes::BCNames & boundaryConditionType=BCTypes::dirichlet,
                              const int & boundaryCondition = allBoundaries,
                              const real & forcing = 0.,
                              const real & time = 0.,
                              const BoundaryConditionParameters & bcParameters 
                                          = Overture::defaultBoundaryConditionParameters(),
			      const int & grid=0);
  

  void applyBoundaryCondition(realMappedGridFunction & u, 
                              const Index & Components,
                              const BCTypes::BCNames & boundaryConditionType,
                              const int & boundaryCondition,
                              const realArray & forcing,
                              const real & time = 0.,
                              const BoundaryConditionParameters & bcParameters 
                                          = Overture::defaultBoundaryConditionParameters(),
			      const int & grid=0);
  
  virtual void applyBoundaryCondition(realMappedGridFunction & u, 
				      const Index & Components,
				      const BCTypes::BCNames & boundaryConditionType,
				      const int & boundaryCondition0,
				      const RealArray & forcing,
				      realArray *forcinga[2][3],
				      const real & time = 0.,
				      const BoundaryConditionParameters & bcParameters 
				      = Overture::defaultBoundaryConditionParameters(),
                                      const int & grid=0 ){} // *wdh* 000502
  
  void applyBoundaryCondition(realMappedGridFunction & u, 
                              const Index & Components,
                              const BCTypes::BCNames & boundaryConditionType,
                              const int & boundaryCondition,
                              const realMappedGridFunction & forcing,
                              const real & time = 0.,
                              const BoundaryConditionParameters & bcParameters 
                                          = Overture::defaultBoundaryConditionParameters(),
			      const int & grid=0);
  
  // fill in the BC's for a coefficient grid function
  void applyBoundaryConditionCoefficients(realMappedGridFunction & coeff, 
					  const Index & Equations,
					  const Index & Components,
					  const BCTypes::BCNames & boundaryConditionType=BCTypes::dirichlet,
					  const int & boundaryCondition= allBoundaries,
					  const BoundaryConditionParameters & bcParameters 
                                          = Overture::defaultBoundaryConditionParameters(),
					  const int & grid=0);

  // fix corners and periodic update: (moved to base class in Overture.v1)
//  void finishBoundaryConditions(realMappedGridFunction & u );


  private  :
  realMappedGridFunction cellVolume;			//this should be outside somewhere eventually
  realMappedGridFunction faceNormal;			//this should be outside somewhere eventually
  realMappedGridFunction faceNormalCG;			//this is the version with CMPGRD storage order
  realMappedGridFunction faceArea; 
  realMappedGridFunction centerNormal;
  realMappedGridFunction centerNormalCG;		//centerNormal with CMPGRD storage order
  realMappedGridFunction vertexJacobian;

	//
	// DISPLAYS
	//

  Display debugDisplay;
  Display createFaceNormalDisplay;
  Display createCellVolumeDisplay;
  Display createFaceAreaDisplay;
  Display createCenterNormalDisplay;
  Display createVertexJacobianDisplay;
  Display faceAverageDisplay;
  Display averageDisplay;
  Display differenceDisplay;
  Display dZeroDisplay;
  Display contravariantVelocityDisplay;
  Display cellsToFacesDisplay;
  Display divDisplay;
  Display gradDisplay;
  Display FCgradDisplay;
  Display getDefaultIndexDisplay;
  Display LaplacianDisplay;
  Display laplacianDisplay;
  Display boundaryConditionDisplay;
  Display cdDisplay;
  Display vorticityDisplay;

  bool isVolumeScaled;		//(default) true if computed derivatives are multiplied by cell volume
  bool useCMPGRDGeometryArrays;	//use CMPGRD geometry arrays in all calcuations
  bool useInternalGeometryArrays;  // use internal geometry arrays in calculations; ordering of indexes is different

  bool cellVolumeDefined;
  bool faceNormalDefined;
  bool faceAreaDefined;
  bool centerNormalDefined;
  bool vertexJacobianDefined;

  bool faceNormalCGDefined;
  bool centerNormalCGDefined;

  realMappedGridFunction Rx;	// links to components of internal geometry arrays
  realMappedGridFunction Ry;
  realMappedGridFunction Rz;
  realMappedGridFunction Sx;
  realMappedGridFunction Sy;
  realMappedGridFunction Sz;
  realMappedGridFunction Tx;
  realMappedGridFunction Ty;
  realMappedGridFunction Tz;

  realMappedGridFunction rX;	// links to components of CMPGRD geometry arrays
  realMappedGridFunction rY;	// initially (950718) these are links to the CMPGRD-ordered
  realMappedGridFunction rZ;	// internal arrays, but eventually the CMPGRD arrays
  realMappedGridFunction sX;	// themselves will be used.
  realMappedGridFunction sY;
  realMappedGridFunction sZ;
  realMappedGridFunction tX;
  realMappedGridFunction tY;
  realMappedGridFunction tZ;

  realMappedGridFunction rXCenter;	// links to components of CMPGRD geometry arrays
  realMappedGridFunction rYCenter;	// initially (950718) these are links to the CMPGRD-ordered
  realMappedGridFunction rZCenter;	// internal arrays, but eventually the CMPGRD arrays
  realMappedGridFunction sXCenter;	// themselves will be used.
  realMappedGridFunction sYCenter;
  realMappedGridFunction sZCenter;
  realMappedGridFunction tXCenter;
  realMappedGridFunction tYCenter;
  realMappedGridFunction tZCenter;

  int numberOfComponents;
  int maximumNumberOfBoundaryConditions;
  int width,halfWidth1,halfWidth2,halfWidth3;
  intArray numberOfBoundaryConditions;
  intArray boundaryCondition;
  intArray componentForBoundaryCondition;

  intArray boundaryConditionValueGiven;
  realArray boundaryConditionValue;

  bool boundaryData;
  realMappedGridFunction boundaryRHS;

  realMappedGridFunction uDotN;

// ***this shouldn't be here
//  bool twilightZoneFlow;
//  OGFunction *twilightZoneFlowFunction;

  bool rectangular;  // will be used to flag a rectangular grid for efficiency
  

  // ============================================================
  // these functions are private because no error checking of arguments is done
  // it is assumed that the routines that call them have done this already.
  // ============================================================

  void setup ();	//reused code used by constructors
  void cleanup ();

  void inputParameterErrorHandler (
			const functionNames functionName,
			const int p0=defaultValue,
			const int p1=defaultValue,
			const int p2=defaultValue,
			const int p3=defaultValue,
			const int p4=defaultValue,
			const int p5=defaultValue,
			const int p6=defaultValue,
			const int p7=defaultValue,
			const int p8=defaultValue,
			const int p9=defaultValue
		      );

  // private boundary condition routine:
  void applyBoundaryCondition(realMappedGridFunction & u, 
                              const Index & Components,
                              const BCTypes::BCNames & boundaryConditionType,
                              const int & boundaryCondition,
                              const real & forcing1,
                              const realArray & forcing2,
                              const realMappedGridFunction & forcing3,
                              const real & time,
                              const BoundaryConditionParameters & bcParameters,
                              const int bcOption  );



  REALMappedGridFunction gradFromCellCenteredInput (	
			const REALMappedGridFunction & phi,   	//returns cell-centered gradient given cell-centered input
			const Index & I1 ,				
			const Index & I2 ,
			const Index & I3 ,
			const Index & Components 
			);

  REALMappedGridFunction gradFromFaceCenteredInput (	
			const REALMappedGridFunction & phi,   	//returns cell-centered gradient given face-centered input
			const Index & I1 ,				
			const Index & I2 ,
			const Index & I3 ,
			const Index & Components
			);
  REALMappedGridFunction FCgradFromCellCenteredInput (	
			const REALMappedGridFunction & phi,   	//returns face-centered gradient given cell-centered input
			const Index & I1 ,				
			const Index & I2 ,
			const Index & I3 ,
			const Index & Components
			);

  REALMappedGridFunction FCgradFromFaceCenteredInput (	
			const REALMappedGridFunction & phi,   	//returns face-centered gradient given face centered input
			const Index & I1 ,				
			const Index & I2 ,
			const Index & I3 ,
			const Index & Components
			);

  bool isInStandardOrdering (const REALMappedGridFunction & u);
  
  void standardOrderingErrorMessage (const REALMappedGridFunction &u, const aString & routineName);

// bool getIsCellCentered (const REALMappedGridFunction &u);		//temporarily replacing Bill's class routine

  void getDefaultIndex (
			const REALMappedGridFunction & u,		// choose Index'es where stuff will be returned
			const int  c0 ,
			const int  c1 ,
			const int  c2 ,
			const int  c3 ,
			const int  c4 ,
                	Index & I1,					// extra in each axis direction
                	Index & I2,
                	Index & I3,
                	const int  extra1=0,
                	const int  extra2=0,
                	const int  extra3=0,
                	const Index & I1From=nullIndex,
                	const Index & I2From=nullIndex,
                	const Index & I3From=nullIndex);

  void getDefaultIndex (                                                 // used for general derivative operators
			const GridFunctionParameters::GridFunctionType & inputGFType,
			const GridFunctionParameters::GridFunctionType & outputGFType,
			Index & I1,
			Index & I2,
			Index & I3,
//			const int component   = 0,              
			const Index & I1input = nullIndex,
			const Index & I2input = nullIndex,
			const Index & I3input = nullIndex
			);
  

  REALMappedGridFunction generalLaplacian(				// laplacian operators
			const LaplacianType laplacianType,
			const REALMappedGridFunction & u,
			const REALMappedGridFunction & scalar,
			const Index & I1 = nullIndex,
			const Index & I2 = nullIndex,
			const Index & I3 = nullIndex,
			const Index & N  = nullIndex
			);

  REALMappedGridFunction GeneralLaplacian(				// laplacian operators
			const LaplacianType laplacianType,
			const REALMappedGridFunction & scalar,
			const Index & I1 = nullIndex,
			const Index & I2 = nullIndex,
			const Index & I3 = nullIndex
			);

  REALMappedGridFunction GeneralLaplacianCoefficients(				// laplacian operators
			const LaplacianType laplacianType,
			const REALMappedGridFunction & scalar,
			const Index & I1 = nullIndex,
			const Index & I2 = nullIndex,
			const Index & I3 = nullIndex
			);

  realMappedGridFunction genericLaplacianCoefficientsForSystems (const LaplacianType lt,
                                                                 const realMappedGridFunction & scalar,
								 const Index & I1 = nullIndex,
								 const Index & I2 = nullIndex,
								 const Index & I3 = nullIndex,
								 const Index & E  = nullIndex,
								 const Index & C  = nullIndex
								 );

  void firstDerivatives (realMappedGridFunction & returnedValue,
                         const int & direction,
			 const GridFunctionParameters & gfParams,
			 const realMappedGridFunction &u,
			 const Index & I1 = nullIndex,
			 const Index & I2 = nullIndex,
			 const Index & I3 = nullIndex,
			 const Index & Components = nullIndex);


  void firstDerivativeCoefficients (realMappedGridFunction & returnedVlaue,
				    const int & dierction,
				    const GridFunctionParameters & gfParams, 
				    const Index & I1 = nullIndex,
				    const Index & I2 = nullIndex,
				    const Index & I3 = nullIndex
				    );
  

  realMappedGridFunction typedDerivative (const realMappedGridFunction & u,
					  const int & direction,
					  const GridFunctionParameters & gfParams,
					  const Index & I1 = nullIndex,
					  const Index & I2 = nullIndex,
					  const Index & I3 = nullIndex,
					  const Index & Components = nullIndex);
  

  realMappedGridFunction untypedDerivative (const realMappedGridFunction & u,
					    const int & direction,
					    const Index & I1 = nullIndex,
					    const Index & I2 = nullIndex,
					    const Index & I3 = nullIndex,
					    const Index & Components = nullIndex);
  

  

  void createFaceNormal     (MappedGrid &mg);				// this should go away
  void createFaceNormalCG   (MappedGrid &mg);
  void createCellVolume     (MappedGrid &mg);				// this should go away
  void createCenterNormal   (MappedGrid &mg);
  void createCenterNormalCG (MappedGrid &mg);
  void createFaceArea       (MappedGrid &mg);
  void createVertexJacobian (MappedGrid &mg);

// *** these are here for compatibility with future Overture.v1
//
//  bool extrapolateInterpolationNeighboursIsInitialized;
//  int numberOfInterpolationNeighbours[3];
//  intArray ipn[3];
//  intArray ipd[3];
//  
//  void findInterpolationNeighbours ()
//  {cout << "ERROR: MappedGridFiniteVolumeOperators::findInterpolationNeighbours called" <<endl;};
      
};
#endif
