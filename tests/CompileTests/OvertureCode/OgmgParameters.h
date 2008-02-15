#ifndef OGMG_PARAMETERS_H
#define OGMG_PARAMETERS_H

// This class holds parameters used by Ogmg.
// You can save commonly used parameter values in an object of this class.
// You can interactively update parameter values.
// To make these parameters known to a particular Ogmg object, use the
// Ogmg::setOgmgParameters function.

#include "Overture.h"
#include "OgesParameters.h"

class Ogmg;  // forward declaration
class GenericGraphicsInterface;

class OgmgParameters
{
 public:  

  enum OptionEnum  // ** remember to change the documentation if you change this list.
  {
    THEnumberOfCycles,                  // number of times to iterate on each level (=1 -> V-cycle, 2=W-cycle)
    THEnumberOfSmooths,                 // (minimum) number of smooths (level)
    THEnumberOfSubSmooths,              // number of sub smooths (grid,level)
    THEsmootherType,                    // type of smooth (grid,level)
    THEsmoothingRateCutoff,             // continue smoothing until smoothing rate is bigger than this
    THEuseDirectSolverOnCoarseGrid,     // if true use Oges, if false use a 'smoother' on the coarse grid.
    THEresidualTolerance, 
    THEerrorTolerance, 
    THEmeanValueForSingularProblem,
    THEmaximumNumberOfIterations,
    THEmaximumNumberOfExtraLevels
  };

  enum SmootherTypeEnum
  {
    Jacobi=0,
    GaussSeidel,
    redBlack,
    lineJacobiInDirection1,
    lineJacobiInDirection2,
    lineJacobiInDirection3,
    lineZebraInDirection1,
    lineZebraInDirection2,
    lineZebraInDirection3,
    alternatingLine,
    numberOfSmoothers
  };
  

  enum BoundaryConditionEnum  // these are the numbers defining values in the cg.boundaryCondition(side,axis) array
  {
    dirichlet=1,
    neumann=2,
    mixed=3,
    equation,
    extrapolation,
    combination
  };

  enum
  {
    allGrids=-99,
    allLevels=-100
  };

  OgmgParameters();
  OgmgParameters(CompositeGrid & cg);  // apply parameters to this grid.
  ~OgmgParameters();

  virtual OgmgParameters& operator=(const OgmgParameters& par);

  int set( CompositeGrid & cg);  // apply parameters to this grid.
  int updateToMatchGrid( CompositeGrid & cg);  
  
  // aString getSolverName();         

  int setParameters( const Ogmg & ogmg);       // set all parameters equal to those values found in ogmg.
  int update( GenericGraphicsInterface & gi, CompositeGrid & cg ); // update parameters interactively

  // ----------------Functions to set parameters -----------------------------
  int set( OptionEnum option, int value=0 );
  int set( OptionEnum option, float value );
  int set( OptionEnum option, double value );
	   		      
  int setMaximumNumberOfIterations( const int max );

  // specify the number of iterations per level (1=V cycle, 2=W cycle)
  int setNumberOfCycles( const int & number, const int & level=allLevels );

  // Set the number of smooths on a given level
  int setNumberOfSmooths(const int numberOfSmooths, const int level);

  int setNumberOfSubSmooths( const int & numberOfSmooths, const int & grid, const int & level=allLevels);

  // Set the tolerance for the residual, iterate until the norm of the residual is 
  // less than "residualTolerance"
  int setResidualTolerance(const real residualTolerance );

  // Set the tolerance for the error, iterate until the norm of the estimated error is 
  // less than "errorTolerance"
  int setErrorTolerance(const real errorTolerance );


  // Indicate if the problem is singular
  int setProblemIsSingular( const bool trueOrFalse=TRUE );
  //
  // set the mean value of the solution for a singular problem
  int setMeanValueForSingularProblem( const real meanValue );
  //
  int setSmootherType(const SmootherTypeEnum & smoother, 
		      const int & grid=allGrids, 
		      const int & level=allLevels );


  int get( OptionEnum option, int & value ) const;
  int get( OptionEnum option, real & value ) const;


  int get( const GenericDataBase & dir, const aString & name);
  int put( GenericDataBase & dir, const aString & name) const;
  
  // print out current values of parameters
  int display(FILE *file = stdout);


 protected:

  int numberOfMultigridLevels() const;  // this is how many levels the parameters thinks exists
  int numberOfComponentGrids() const;  // this is how many grids the parameters thinks exists
  

  void init();
  int initializeParameters(CompositeGrid & cg );
  
  int set( OptionEnum option, int value, real rvalue );
  int get( OptionEnum option, int & value, real & rvalue ) const;

  IntegerArray numberOfCycles;      // number of times to iterate on each level (=1 -> V-cycle, 2=W-cycle)
  IntegerArray numberOfSmooths;         // (minimum) number of smooths (level)
  IntegerArray numberOfSubSmooths;      // number of sub smooths (grid,level)
  IntegerArray smootherType;            // type of smooth (grid,level)

  real smoothingRateCutoff;             // continue smoothing until smoothing rate is bigger than this
  bool useDirectSolverOnCoarseGrid;     // if false use a 'smoother' on the coarse grid.
  bool problemIsSingular;
  
  real residualTolerance, errorTolerance, meanValueForSingularProblem;
  int maximumNumberOfIterations;

  OgesParameters ogesParameters;
  CompositeGrid *cgPointer;

  aString *smootherName;
  
  bool interpolateTheDefect;
  int maximumNumberOfExtraLevels;
  bool autoSubSmoothDetermination;
  int maximumNumberOfLevels;

  int boundaryAveragingOption;
  int useSymmetryCornerBoundaryCondition;

  friend class Ogmg;
};

#endif
