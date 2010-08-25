#ifndef OGMG_H
#define OGMG_H "Ogmg.h"

#include "Overture.h"
#include "Oges.h"
#include "CompositeGridOperators.h"
#include "displayLowercase.h"
#include "PlotStuff.h"
#include "OgmgParameters.h"

Index IndexBB(int base, int bound, int stride=1 );
Index IndexBB(Index I, const int stride );

class TridiagonalSolver;  // forward declaration

//-------------------------------------------------------------------------------------------------------
//    Overlapping Grid Multigrid Solver
//-------------------------------------------------------------------------------------------------------
class Ogmg
{
 public:


  enum BoundaryConditionEnum  // these are the numbers defining values in the cg.boundaryCondition(side,axis) array
  {
    dirichlet=1,
    neumann=2,
    mixed=3,
    equation,
    extrapolation,
    combination
  };

  enum TransferTypesEnum
  {
    fullWeighting,
    injection
  };

  enum
  {
    allGrids=-99,
    allLevels=-100
  };

  Ogmg();
  Ogmg( CompositeGrid & mg, PlotStuff *ps=0);
  ~Ogmg();
  
  
  // set and get parameters for Ogmg using the next object
  OgmgParameters parameters;
  
  // set parameters equal to another parameter object.
  int setOgmgParameters(OgmgParameters & parameters );
  
  int setOrderOfAccuracy(const int & orderOfAccuracy);

  virtual real sizeOf( FILE *file=NULL ) const ; // return number of bytes allocated by Oges, print info to a file

  int chooseBestSmoother();
  //
  // Return the "mean" value of a grid function. Use a particular
  // definition of mean.
  // 
  real getMean(realCompositeGridFunction & u);
  int getNumberOfIterations() const;
  real getMaximumResidual() const;

  int update( GenericGraphicsInterface & gi ); // update parameters interactively
  int update( GenericGraphicsInterface & gi, CompositeGrid & cg ); // update parameters interactively

  void updateToMatchGrid( CompositeGrid & mg );
  //
  // Supply the coefficients (for all multigrid levels)
  //
  int setCoefficientArray( realCompositeGridFunction & coeff );
  void set( PlotStuff *ps );
  int solve( realCompositeGridFunction & u, realCompositeGridFunction & f );

  int cycle(const int & level, const int & iteration, real & maximumDefect);  // cycle at level l


  void printStatistics(FILE *file=stdout) const;

  int smoothTest(realCompositeGridFunction & u0,
                 realCompositeGridFunction & f0 );

  int coarseToFineTest(realCompositeGridFunction & u,
                       realCompositeGridFunction & f);

  int fineToCoarseTest(realCompositeGridFunction & u,
                       realCompositeGridFunction & f);
  
  int bcTest(realCompositeGridFunction & u,
             realCompositeGridFunction & f );
  
  realCompositeGridFunction & getDefect() { return defectMG;} 

  // int get( const GenericDataBase & dir, const aString & name);
  // int put( GenericDataBase & dir, const aString & name) const;


  void smooth(const int & level);
  void smoothJacobi(const int & level, const int & grid);
  void smoothGaussSeidel(const int & level, const int & grid);
  void smoothRedBlack(const int & level, const int & grid);
  void smoothLine(const int & level, const int & grid, const int & direction );
  void alternatingLineSmooth(const int & level, const int & grid);
  
  void defect(const int & level);
  void defect(const int & level, const int & grid);
  
  void fineToCoarse(const int & level);
  void fineToCoarse(const int & level, const int & grid);

  void coarseToFine(const int & level);
  void coarseToFine(const int & level, const int & grid);

  void getDefect(const int & level, 
                 const int & grid, 
		 realArray & f,      // could be const, except reshape needed
		 realArray & u,      // could be const, except reshape needed
		 const Index & I1,
		 const Index & I2,
		 const Index & I3,
		 realArray & defect,
		 const int lineSmoothOption=-1);

  void evaluateTheDefectFormula(const int & level, 
				const int & grid, 
				const realArray & c,
				const realArray & u,  
				const realArray & f, 
				realArray & defect, 
				MappedGrid & mg,
				const Index & I1,
				const Index & I2,
				const Index & I3,
				const Index & I1u,
				const Index & I2u,
				const Index & I3u,
                                const int lineSmoothOption);
  
  int interpolate(realCompositeGridFunction & u, const int & grid =-1 );

  // here is the new way:
//   int assignBoundaryConditions(const int & level,  
// 			       const int & grid, 
// 			       RealMappedGridFunction & u, 
// 			       RealMappedGridFunction & f );

  int applyBoundaryConditions(const int & level,  
			      const int & grid, 
                              RealMappedGridFunction & u, 
                              RealMappedGridFunction & f );
  int applyBoundaryConditions( const int & level, RealCompositeGridFunction & u, RealCompositeGridFunction & f );
  
  // form coarse grid operator by averaging the fine grid operator
  int operatorAveraging(RealCompositeGridFunction & coeff, const int & level);
  int operatorAveraging(RealMappedGridFunction & coeffFine,
			RealMappedGridFunction & coeffCoarse,
			const IntegerArray & coarseningRatio,
			int grid  =0,
			int level =0 );
  
  int averageCoefficients(Index & I1, Index & I2, Index & I3,
			  Index & I1p, Index & I2p, Index & I3p,
			  Index & J1, Index & J2, Index & J3,
			  TransferTypesEnum option,
			  const realArray & cFine, 
			  realArray & cCoarse,
                          int side = -1,
                          int axis = -1 );
  
  int markGhostPoints( CompositeGrid & cg );

 public:
  Oges directSolver;                  // direct solver for coarsest level

  // protected:   // **** temporary

  realCompositeGridFunction uMG,fMG;   // local reference copies
  realCompositeGridFunction defectMG;  // holds defect
  realCompositeGridFunction cMG;     // coefficients stored here
  CompositeGrid mgcg;
  
 protected:

  enum SmootherTypeEnum
  {
    Jacobi                 =OgmgParameters::Jacobi,
    GaussSeidel            =OgmgParameters::GaussSeidel,
    redBlack               =OgmgParameters::redBlack,
    lineJacobiInDirection1 =OgmgParameters::lineJacobiInDirection1,
    lineJacobiInDirection2 =OgmgParameters::lineJacobiInDirection2,
    lineJacobiInDirection3 =OgmgParameters::lineJacobiInDirection3,
    lineZebraInDirection1  =OgmgParameters::lineZebraInDirection1,
    lineZebraInDirection2  =OgmgParameters::lineZebraInDirection2,
    lineZebraInDirection3  =OgmgParameters::lineZebraInDirection3,
    alternatingLine        =OgmgParameters::alternatingLine,
    numberOfSmoothers
  };

//  int set( OptionEnum option, int value, real rvalue );
//  int get( OptionEnum option, int & value, real & rvalue ) const;

  
   int orderOfAccuracy;         
//   intArray numberOfSmooths;                 // (minimum) number of smooths (level)
//   intArray numberOfSubSmooths;              // number of sub smooths (grid,level)
//   intArray smootherType;                    // type of smooth (grid,level)
//   real smoothingRateCutOff;                 // continue smoothing until smoothing rate is bigger than this
//   bool useDirectSolverOnCoarseGrid;         // if false use a 'smoother' on the coarse grid.
//   intArray numberOfIterations;   // number of times to iterate on each level (=1 -> V-cycle, 2=W-cycle)
//   real residualTolerance, errorTolerance, meanValueForSingularProblem;
//   int maximumNumberOfIterations;
  
//   bool problemIsSingular;

  int width1,width2,width3,halfWidth1,halfWidth2,halfWidth3;
  realArray workUnits;           // number of work units used per level (one cycle only)

  TridiagonalSolver ****tridiagonalSolver;  // [level][grid][axis]
  intArray lineSmoothIsInitialized;   // (axis,grid,level)

  CompositeGridOperators *operatorsForExtraLevels;   // array of operators for extra levels.
  Interpolant *interpolant; 
  IntegerArray interpolantWasCreated;

  void setup(CompositeGrid & mg );
  void setMean(realCompositeGridFunction & u, const real meanValue=0.);
  real l2Norm(const realCompositeGridFunction & e );
  real l2Norm(const realMappedGridFunction & e );
  real maxNorm(const realCompositeGridFunction & e );
  
 public:
  int initializeBoundaryConditions(realCompositeGridFunction & coeff);
  int initializeConstantCoefficients();

  PlotStuff *ps;
  PlotStuffParameters psp;
  
  static int debug;
  static FILE *debugFile;           // debug file
  static FILE *infoFile;
  static FILE *checkFile;

  BoundaryConditionParameters bcParams;

 protected:

  bool initialized; // TRUE if initialized with a grid.
  
  real workUnit, timeForAddition, timeForMultiplication, timeForDivision;
  int numberOfGridPoints;
  int numberOfCycles;               // total number over all solves
  int numberOfIterations;           // number for the last call to solve
  intArray boundaryCondition;
  intArray isConstantCoefficients;  // isConstantCoefficients(grid)
  RealArray constantCoefficients;  
  int numberOfExtraLevels;
  real maximumResidual;
  
  enum Timing
  {
    timeForDefect=0,
    timeForSmooth,
    timeForFineToCoarse,
    timeForCoarseToFine,
    timeForDirectSolver,
    timeForSolve,
    timeForInterpolation,
    timeForBoundaryConditions,
    numberOfThingsToTime  // counts the number of elements in this list
  };
  RealArray tm;     // for timings
  char buff[100];

  RealArray defectRatio;    // (grid,level) holds ratio of defect(grid,level) / min( defect(*,level) )
  realCompositeGridFunction uOld;

  RealArray alpha;                       // compatibility value for singular problems.
  realCompositeGridFunction rightNullVector;  // right null vector used for singular problems.

  int createNullVector();
  real rightNullVectorDotU( const int & level, const RealCompositeGridFunction & u );

  int buildExtraLevels(CompositeGrid & mg);
  int getInterpolationCoordinates(CompositeGrid & cg0, // finer grid
				  CompositeGrid & cg1, // new coarser grid
				  int i,               // check this point
				  int grid,
                                  int iv[],
				  int jv[],
				  realArray & r );
  int getInterpolationStencil(CompositeGrid & cg0,
			      CompositeGrid & cg1,
			      int i,
			      int iv[3],
			      int grid,
                              int l,
			      intArray & inverseGrid,
			      intArray & interpoleeGrid,
			      intArray & interpoleeLocation,
			      intArray & interpolationPoint,
			      intArray & variableInterpolationWidth,
			      realArray & interpolationCoordinates,
			      realArray & inverseCoordinates );
  
};


#endif
