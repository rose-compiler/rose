/*  -*-Mode: c++; -*-  */
#ifndef PROJECTION_H
#define PROJECTION_H

#define V15
 
#include <assert.h>
#include <CompositeGrid.h>
#include <CompositeGridFunction.h>
#include <Oges.h>
#include <NameList.h>
#include <Interpolant.h>
#include <OGgetIndex.h>
#include <PlotStuff.h>
#include "davidsReal.h"
#include "axisDefs.h"
#include "loops.h"
#include "Display.h"
#include "CompositeGridFiniteVolumeOperators.h"
#include "MappedGridFiniteVolumeOperators.h"
//#include "ThreeLevelMovingGrids.h" GONE in V16
#include "DynamicMovingGrids.h"

// ... later we'll implement a more general MovingGrids class, meanwhile use ThreeLevelMovingGrids

#ifdef V15
typedef DynamicMovingGrids MovingGrids;
#else
typedef ThreeLevelMovingGrids MovingGrids;
#endif

//--------------------------------------------------------------------------------
//
// class Projection
// The purpose of this class is to define a Projection operator on
// compositeGrid's for finite volume codes
// D. L. Brown 950801
//
//--------------------------------------------------------------------------------

class Projection
{
  public :

	// ========================================
	// PUBLIC DATA
	// ========================================

  static const Index nullIndex;                      //some static consts that are used all over the place
  static const Range all;
  static bool projectionDebug;

        // ========================================
        // ENUMS
        // ========================================

  enum BoundaryConditionType                         //this enum is used to specify boundary condition types for the elliptic solution and velocities
  {
    noBoundaryConditionDefined = -1,
    cheapNeumann,
    normalDerivativeGiven,
    valueGiven,
    normalValueGiven,
    extrapolateAllComponents,
    homogeneousNeumann,
    fixedWallNeumann,
    movingWallNeumann,
    numberOfBoundaryConditionTypes
    };

  enum ExactVelocityType                             //enum to flag the ExactVelocityType
  {
    noExactVelocity,
    zeroExactVelocity,
    periodicVelocity,
    polynomialVelocity,
    shearLayers,
    numberOfExactVelocityTypes
  };

  enum ProjectionType                                //enum to flag the projection type
  {
    noProjection = -1,
    approximateProjection,
    macProjection,
    macProjectionOfNormalVelocity,
    numberOfProjectionTypes
    };

  enum EllipticSolverParameter
  {
    conjugateGradientPreconditioner,
    conjugateGradientType,
    conjugateGradientNumberOfIterations,
    conjugateGradientNumberOfSaveVectors,
    fillinRatio,
    zeroRatio,
    fillinRatio2,
    harwellTolerance,
    matrixCutoff,
    iterativeImprovement,
    preconditionBoundary,
    solverType,
    sorNumberOfIterations,
    sorOmega,
    numberOfEllipticSolverParameters
    }; 

  enum
  {
    undefinedValue=-999,
    defaultValue=-998,
    forAll=-997
  };

      static real undefinedRealValue;
  

  GridFunctionParameters::GridFunctionType  general;
  GridFunctionParameters::GridFunctionType  vertexCentered    ;
  GridFunctionParameters::GridFunctionType  cellCentered      ;
  GridFunctionParameters::GridFunctionType  faceCenteredAll   ;
  GridFunctionParameters::GridFunctionType  faceCenteredAxis1 ;
  GridFunctionParameters::GridFunctionType  faceCenteredAxis2 ;
  GridFunctionParameters::GridFunctionType  faceCenteredAxis3 ;
  GridFunctionParameters::GridFunctionType  defaultCentering  ;
  
  GridFunctionParameters::GridFunctionTypeWithComponents cellCenteredWith0Components ;
  GridFunctionParameters::GridFunctionTypeWithComponents cellCenteredWith1Component  ;
  GridFunctionParameters::GridFunctionTypeWithComponents cellCenteredWith2Components ;

  GridFunctionParameters::GridFunctionTypeWithComponents faceCenteredAllWith0Components ;
  GridFunctionParameters::GridFunctionTypeWithComponents faceCenteredAllWith1Component ;
  GridFunctionParameters::GridFunctionTypeWithComponents faceCenteredAllWith2Components ;
  
    
	// ========================================
	// CONSTRUCTORS
	// ========================================
  Projection ();
  Projection (CompositeGrid & cg);  // OBSOLETE 981111

//This constructor should be declared explicit
//explicit Projection (CompositeGridFiniteVolumeOperators * operators_);
  Projection (CompositeGridFiniteVolumeOperators * operators_);
  Projection (CompositeGrid & cg, 
	      CompositeGridFiniteVolumeOperators * operators_,
	      MovingGrids * movingGridsPointer_=NULL);  //...set a pointer to a MovingGrids object; used for BCs
  ~Projection ();
  
  void updateToMatchGrid (CompositeGrid & cg);        //used to update a Projection object to match a (new) compositeGrid
//  void updateToMatchGridAndOperators (CompositeGrid & cg, 
//				      CompositeGridFiniteVolumeOperators * operators0);
  void setOperators (CompositeGridFiniteVolumeOperators *operators_);
  void setMovingGridsPointer (MovingGrids * movingGridsPointer_);



  
  
  
	// ========================================
	// GENERAL CLASS FUNCTIONS
	// ========================================
  
  // these functions take
  //      a cellCentered uStar(all,all,all,Index(0,numberOfDimensions)), presumed cellCentered velocities, or
  //      a faceCentered uStar(all,all,all,faceRange,Index(0,numberOfDimensions)), presumed faceCentered velocities, or
  //      a faceCentered uStar(all,all,all,faceRange), presumed faceCentered NORMAL velocities
  // and return the projection potential phi; Does not modify uStar
  //
  void project (                                                             // DEBUGGING VERSION that passes in plotting debug info
		realCompositeGridFunction & uStar,
		realCompositeGridFunction & phi,
		PlotStuff * ps,
		PlotStuffParameters * psp,
		bool & PLOT_ON,
		const int & level = 0,
		const real & velocityTime = undefinedRealValue,
		const real & pressureTime = undefinedRealValue);          //const-coeff projection

  void project (                                                             //elliptic solve only, don't return a velocity
		realCompositeGridFunction & uStar,
		realCompositeGridFunction & phi0,
		const int & level = 0,
		const real & velocityTime = undefinedRealValue,
		const real & pressureTime = undefinedRealValue
		);
  
  // these functions take
  //      a cellCentered uStar(all,all,all,Index(0,numberOfDimensions)), presumed cellCentered velocities, or
  //      a faceCentered uStar(all,all,all,faceRange,Index(0,numberOfDimensions)), presumed faceCentered velocities
  // and return the projected velocity of the same dimensions
  // 


  realCompositeGridFunction project (                                          // DEBUGGING VERSION
                                     realCompositeGridFunction & uStar,
				     PlotStuff * ps,
				     PlotStuffParameters * psp,
				     bool & PLOT_ON,
				     const int & level = 0,
				     const real & velocityTime = undefinedRealValue,
				     const real & pressureTime = undefinedRealValue);
      

  realCompositeGridFunction project (                                          // elliptic solve and return the projected velocity
                                     realCompositeGridFunction & uStar,
				     const int & level = 0,
				     const real & velocityTime = undefinedRealValue,
				     const real & pressureTime = undefinedRealValue
				     );


  void setDensity (realCompositeGridFunction & rho0); //set new density for variable density projection

  void associateCMPGRDBoundaryCondition (            // associate a CMPGRD boundaryCondition with a Projection boundaryConditionType
                                         const int bcCMPGRD,
					 const BoundaryConditionType pressureBC
					 );
  void associateOvertureBoundaryCondition (            // associate a CMPGRD boundaryCondition with a Projection boundaryConditionType
                                         const int bcCMPGRD,
					 const BoundaryConditionType pressureBC
					 );
  

  void interactivelyAssociateBoundaryConditions ();  //interactive interface to associateCMPGRDBoundaryCondition
  void boundaryConditionWizard ();

  void setVelocityBoundaryConditionValue (           //set a value for a velocity component on a boundary
					  real value = 0.,
					  int component = forAll,
					  int side = forAll,
					  int axis = forAll,
					  int grid = forAll,
					  bool trueOrFalse = TRUE
					  );

// interface to setting Oges parameters: overloaded to simplify syntax


  void ellipticSolverParameterWizard ();
  
  void setEllipticSolverParameter (const EllipticSolverParameter & parameterName, const Oges::conjugateGradientPreconditioners & type);
  void setEllipticSolverParameter (const EllipticSolverParameter & parameterName, const Oges::conjugateGradientTypes & type);
  void setEllipticSolverParameter (const EllipticSolverParameter & parameterName, const int & value);
  void setEllipticSolverParameter (const EllipticSolverParameter & parameterName, const real & value);
  void setEllipticSolverParameter (const EllipticSolverParameter & parameterName, const Oges::solvers & type);
      
  realCompositeGridFunction getPressure ();
  realCompositeGridFunction getPhi ();
  void setTimestep (const real & dt);
  void setIsIncrementalPressureFormulation (const bool & trueOrFalse = FALSE);
  void setPreviousPressureTimeLevel (const real & previousTime);

  void setTwilightZoneFlow (const bool TrueOrFalse=LogicalTrue);
  void setTwilightZoneFlowFunction (OGFunction & TwilightZoneFlowFunction);  // ***obsolete 981123
  void setTwilightZoneFlowFunction (OGFunction * tzFunction);
  
  void setEllipticCompatibilityConstraint (const bool trueOrFalse);
  void setIsVariableDensityProjection (const bool trueOrFalse = TRUE);
  realCompositeGridFunction getPerturbedVelocity ();
  void  setPerturbedVelocity (ExactVelocityType & exactVelocityType, real & perturbationSize);
  realCompositeGridFunction getExactVelocity ();
  realCompositeGridFunction getExactPhi ();
  realCompositeGridFunction getExactGradient ();
  
// ========================================
  private:
// ========================================

// ========================================
// private ENUMS
// ========================================



  enum VelocityBoundaryConditionType
  {
    unprojectedVelocityBC,
    projectedVelocityBC
    };

// ... private int grid stuff  
  int numberOfDimensions;
  int numberOfComponentGrids;
  int maximumNumberOfOvertureBCs;

  int maximumNumberOfBoundaryConditions;
  
  intArray velocityBoundaryConditionValueGiven;
  realArray velocityBoundaryConditionValue;

// ... private flags
  bool firstCallToUTMG;
  bool operatorsAreSet;

  bool twilightZoneFlow;
  OGFunction *twilightZoneFlowFunction;
  bool isIncrementalPressureFormulation;
  real previousTime;
      
  bool useExactVelocity;
  bool ellipticCompatibilityConstraintSet;
  bool isVariableDensityProjection;            // default is FALSE, i.e. const coeff projection
  bool reinitializeCoefficients;
  bool timestepIsSet;

  bool movingGridsPointerIsSet;
  MovingGrids * movingGridsPointer;

// .. private REAL params
  real timestep;

// ... private realCompositeGridFunction's
  realCompositeGridFunction exactVelocity;
  realCompositeGridFunction perturbedVelocity;
  realCompositeGridFunction exactGradient;
  realCompositeGridFunction exactPhi;
  realCompositeGridFunction phi;
  realCompositeGridFunction pressure;
  realCompositeGridFunction coefficients;
  realCompositeGridFunction density;
  
// ... private info about BCs
  BoundaryConditionType * listOfBCs;          // list of boundary condition type conversions
  BoundaryConditionType * listOfVelocityBCs;

// ... (private) elliptic solver  
  Oges ellipticSolver;

// ... private elliptic solver info
  bool iterativeImprovementValue;                                              bool iterativeImprovementReset;
  bool preconditionBoundaryValue;                                              bool preconditionBoundaryReset;
  Oges::conjugateGradientPreconditioners conjugateGradientPreconditionerValue; bool conjugateGradientPreconditionerReset; 
  Oges::conjugateGradientTypes conjugateGradientTypeValue;                     bool conjugateGradientTypeReset;
  int conjugateGradientNumberOfIterationsValue;                                bool conjugateGradientNumberOfIterationsReset;
  int conjugateGradientNumberOfSaveVectorsValue;                               bool conjugateGradientNumberOfSaveVectorsReset;
  int sorNumberOfIterationsValue;                                              bool sorNumberOfIterationsReset;
  real fillinRatioValue;                                                       bool fillinRatioReset;
  real zeroRatioValue;                                                         bool zeroRatioReset;
  real fillinRatio2Value;                                                      bool fillinRatio2Reset; 
  real harwellToleranceValue;                                                  bool harwellToleranceReset;
  real matrixCutoffValue;                                                      bool matrixCutoffReset;
  real sorOmegaValue;                                                          bool sorOmegaReset; 
  Oges::solvers solverTypeValue;                                               bool solverTypeReset;


// ... private big grid stuff
  CompositeGrid compositeGrid;  
  CompositeGridFiniteVolumeOperators *opPointer;
//  CompositeGridFiniteVolumeOperators operators;
//  CompositeGridFiniteVolumeOperators operatorsForVelocity;  //These operators just used for BCs on velocity in projection

// ... private Display's  
  Display projectionDisplay;

// ... private Functions
  void initializeNonGridItems ();
  void cleanup ();
  void updateToMatchGridOnly (CompositeGrid & cg);
  

  void formLaplacianCoefficients ();
  void formEllipticRightHandSide (realCompositeGridFunction & rhs, 
				  realCompositeGridFunction & uStar,
				  const int & level,
				  const real & velocityTime,
				  const real & pressureTime,
				  PlotStuff * ps,
				  PlotStuffParameters * psp,
				  bool & PLOT_ON	     
				  );
  
  void applyLaplacianBoundaryConditions ();  //apply the boundary conditions for the elliptic matrix
  
  void applyRightHandSideBoundaryConditions (                                       // apply boundary conditions to div.uStar: OBSOLETE
                                             realCompositeGridFunction & ellipticRHS,
					     const realCompositeGridFunction & uStar,
					     const int & level = 0,
					     const real & velocityTime = undefinedRealValue,
					     const real & pressureTime = undefinedRealValue
					     );
                        
  void applyUnprojectedVelocityBoundaryConditions (                       // apply BCs to velocity before projection
                                                   realCompositeGridFunction &uStar
						   );
  
  void applyVelocityBoundaryConditions (                                   //apply BCS to velocity after projection
						  realCompositeGridFunction &uStar
						  );

  void applyVelocityBCs (realCompositeGridFunction &uStar,  VelocityBoundaryConditionType vbcType);
                                                                          //worker routine for apply*VelocityBoundaryConditions
  

  void updateEllipticSolverToMatchGrid (CompositeGrid & cg);

  int getMaximumCMPGRDBC (); //get the max CMPGRD bc so we can dimension listOfBCs

  ProjectionType getProjectionType (const realCompositeGridFunction & uStar);  // figure out projection type from input velocity type

  void setPhi (realCompositeGridFunction & uStar,           // this function does the actual elliptic setup and solve
               PlotStuff * ps,
	       PlotStuffParameters * psp,
	       bool & PLOT_ON,
	       const int & level,
	       const real & velocityTime,
	       const real & pressureTime
	       );
  
  
  void checkEllipticConstraint (const realCompositeGridFunction & phi_);
  

};  
#endif
  
  



