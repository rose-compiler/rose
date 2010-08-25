#include "Overture.h"
// #include "Mapping.h"
#include "MappedGridOperators.h"
#include "PlotStuff.h"

class GenericGraphicsInterface;
class DataPointMapping;
class PlotStuff;
class TridiagonalSolver;


class EllipticGridGenerator
{
  // A class to solve pde using the multigrid method

 public:

  int domainDimension, rangeDimension;

  enum BoundaryConditionTypes
  {
    dirichlet=1,
    slipOrthogonal=2,
    noSlipOrthogonalAndSpecifiedSpacing=3,
    noSlipOrthogonal,
    freeFloating
  };

  enum SmoothingTypes
  {
    jacobiSmooth,
    redBlackSmooth,
    lineSmooth,
    line1Smooth,
    line2Smooth,
    line3Smooth,
    zebraSmooth,
    numberOfSmoothingTypes
  };
  


  EllipticGridGenerator();

  EllipticGridGenerator(const EllipticGridGenerator & oldEllipticGridGenerator); // The copy constructor

  ~EllipticGridGenerator();    // The destructor


  // perform iterations
  int generateGrid();

  void setup(Mapping & map, Mapping *projectionMapping=NULL );

  
  // return the current solution.
  const RealMappedGridFunction & solution() const;
  
  // supply a weight function for adaptation.
  int weightFunction( RealMappedGridFunction & weight );
  
  // supply a starting grid.
  int startingGrid(const realArray & u0,
                   const realArray & r0 = Overture::nullRealDistributedArray(),
                   const IntegerArray & indexBounds=Overture::nullIntArray() );

  // Interactively choose parameters and compute elliptic grid.
  int update(DataPointMapping & dpm,
             GenericGraphicsInterface *gi = NULL , 
	     GraphicsParameters & parameters =Overture::defaultGraphicsParameters() );

 protected:

//  void applyBC(realArray &u1, realArray &v1, int i);


  int applyBoundaryConditions(const int & level,
			      RealMappedGridFunction & uu );

  int coarseToFine(const int & level, 
                   const RealMappedGridFunction & uCoarse, 
                   RealMappedGridFunction & uFine,
		   const bool & isAGridFunction = FALSE );

  int defineBoundaryControlFunction();

  int defineSurfaceControlFunction();
  
  int determineBoundarySpacing(const int & side, 
                               const int & axis,
			       real & averageSpacing,
			       real & minimumSpacing,
			       real & maximumSpacing );

  // dot product
  realArray dot( const realArray & a, 
                 const realArray & b, 
		 const Index & I1=nullIndex, 
		 const Index & I2=nullIndex, 
		 const Index & I3=nullIndex);
  
  int estimateUnderRelaxationCoefficients();
  
  int fineToCoarse(const int & level, 
                   const RealMappedGridFunction & uFine, 
                   RealMappedGridFunction & uCoarse,
		   const bool & isAGridFunction = FALSE );

  // get coefficients of the elliptic system
  int  getCoefficients(realArray & coeff, 
                       const Index & J1, 
		       const Index & J2, 
		       const Index & J3,
		       const realArray & ur, 
		       const realArray & us,
                       const realArray & ut = Overture::nullRealDistributedArray() );
  
  int getControlFunctions(const int & level );

  int jacobi(const int & level, 
             RealMappedGridFunction & uu );

  int restrictMovement(const int & level,
                       const RealMappedGridFunction & u0, 
		       RealMappedGridFunction & u1,
		       const Index & I1 =nullIndex, 
		       const Index & I2 =nullIndex, 
		       const Index & I3 =nullIndex);
  

  // For smoothing the weight function:
  int smoothJacobi( RealMappedGridFunction & w,  const int & numberOfSmooths= 4 );

  int lineSmoother(const int & direction,
                   const int & level,
		   RealMappedGridFunction & u );

  int multigridVcycle(const int & level );

  int redBlack(const int & level, 
               RealMappedGridFunction & uu );

  void getResidual(realArray &resid1, 
                   const int & level);

  void getResidual(realArray &resid1, 
                   const int & level,
		   Index Jv[3],
		   realArray & coeff,
		   const bool & computeCoefficients =TRUE,
		   const bool & includeRightHandSide=TRUE,
		   const bool & computeControlFunctions=TRUE,
		   const SmoothingTypes & lineSmoothType =jacobiSmooth  );

  int projectBoundaryPoints(const int & level,
			    RealMappedGridFunction & uu, 
			    const int & side,  
			    const int & axis,
			    const Index & I1,
			    const Index & I2,
			    const Index & I3 );
  
  // this periodic update knows about the derivativePeriodic case
  int periodicUpdate( RealMappedGridFunction & x, 
		      const Range & C=nullRange,
		      const bool & isAGridFunction = TRUE  );

  int plot( const RealMappedGridFunction & v, const aString & label );
  
  realArray signOf(const realArray & uarray);

  int smooth(const int & level, 
             const SmoothingTypes & smoothingType,
	     const int & numberOfSubIterations =1 );

  int stretchTheGrid(Mapping & mapToStretch);

  int updateForNewBoundaryConditions();
  
  void updateRightHandSideWithFASCorrection(int level );


  Mapping *userMap;                     // Defines mapping from r -> x 
  Mapping *projectionMap;               // Use this mapping when projecting boundaries.
  Mapping *boundaryProjectionMap[2][3]; // points to Mappings to use to project boundaries.

  IntegerArray subSurfaceIndex;
  realArray subSurfaceNormal;

  int numberOfLevels;
  int maximumNumberOfLevels;
  int maximumNumberOfIterations;
  
  SmoothingTypes smoothingMethod; 

  Mapping *map;    // for unit interval
  MappedGrid  *mg;
  MappedGridOperators  *operators;
  realMappedGridFunction *u;

  Range Rx,Rr; 

  RealMappedGridFunction *source;
  RealMappedGridFunction *rhs, *w;   //The right hand side and linearized solution

  bool userWeightFunctionDefined;
  RealMappedGridFunction weight;     // weight function for adaption.
   
  RealArray dx;

  real residualTolerance;  // convergence criteria for the maximum residual

  //for the smoother
  real omega;                   // Optimal omega for underelaxed jacobi
  RealArray omegaMax;           // maximum omega as determined from the control functions.
  
  // when there is a gridBc=3
  int numberOfPeriods;          // For doing many sheets in the periodic case
  int useBlockTridiag;

  intArray boundaryCondition;
  realArray boundarySpacing;            //The specified boundary thickness

  RealMappedGridFunction *rBoundary;        // holds unit square coordinates of boundary values.
  RealMappedGridFunction *xBoundary;        // holds x values corresponding to rBoundary

  IntegerArray gridIndex;     // holds interior points plus boundary where equations are applied.
  int numberOfLinesOfAttraction;
  IntegerArray lineAttractionDirection;
  RealArray lineAttractionParameters;

  int numberOfPointsOfAttraction;
  RealArray pointAttractionParameters;

  real normalCurvatureWeight;    // for surface control function.
  real residualNormalizationFactor;  // scaling for residual 
  
  bool useNewStuff;

  int debug;

  TridiagonalSolver *tridiagonalSolver;
  
  bool controlFunctions;         // true if there are control functions.
  bool controlFunctionComputed;
  bool applyBoundarySourceControlFunction;

  PlotStuff *ps;
  PlotStuffParameters psp;
  
  int initializeParameters();
  real maximumResidual, previousMaximumResidual;

  int numberOfCoefficients;  // number of coefficients int the coeff array
  char buff[80];
  FILE *debugFile;

  aString smootherNames[numberOfSmoothingTypes];

  real work;
  
};
