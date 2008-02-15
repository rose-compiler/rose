#ifndef OGES_PARAMETERS_H
#define OGES_PARAMETERS_H

// This class holds parameters used by Oges.
// You can save commonly used parameter values in an object of this class.
// You can interactively update parameter values.
// To make these parameters known to a particular Oges object, use the
// setOgesParameters function.

  //
  //

#include "Overture.h"

class Oges;  // forward declaration
class GenericGraphicsInterface;
class OgmgParameters;

class OgesParameters
{
 public:  

  enum OptionEnum  // ** remember to change the documentation if you change this list.
  {
    THEabsoluteTolerance,
    THEbestIterativeSolver,  // choose the 'best' iterative solver and options.
    THEbestDirectSolver,     // choose the 'best' direct solver and options.
    THEcompatibilityConstraint,
    THEfillinRatio,
    THEfillinRatio2,
    THEfixupRightHandSide,
    THEgmresRestartLength,
    THEharwellPivotingTolerance,
    THEincompleteLUExpectedFill,
    THEincompleteLUDropTolerance,       // =dt for ILUT(dt), ILUTK(dt,k) (from DH preconditioners)
    THEincompleteLUSparseACoefficient,  // =sparseA for sparsifying matrix prior to ILU (from DH)
    THEincompleteLUSparseFCoefficient,  // =sparseF for sparsifying factorization after ILU (from DH)
    THEincompleteLUTypeInDH,            // =ilu_type in DH preconditioners
    THEiterativeImprovement,
    THEkeepCoefficientGridFunction, // keep a reference to the user's coeff grid function
    THEkeepSparseMatrix,            // keep ia,ja,a sparse matrix even it not needed by the solver
    THEmatrixCutoff,
    THEmatrixOrdering,
    THEmaximumInterpolationWidth,
    THEmaximumNumberOfIterations,
    THEminimumNumberOfIterations,
    THEnullVectorScaling,
    THEnumberOfIncompleteLULevels,
    THEsolveForTranspose,
    THEpreconditioner,
    THEremoveSolutionAndRHSVector,      // de-allocate sol and rhs vector after every solve
    THEremoveSparseMatrixFactorization, // de-allocate any factorization info after every solve.
    THErelativeTolerance,
    THErescaleRowNorms,
    THEsolverType,
    THEsolverMethod,
    THEtolerance,
    THEzeroRatio
  };
  
  enum SolverEnum
  {
    defaultSolver,
    sor,
    yale,
    harwell,
    SLAP,
    PETSc,
    multigrid,
    userSolver1, // these are reserved for new user defined solvers.
    userSolver2,
    userSolver3,
    userSolver4,
    userSolver5
  };


  enum SolverMethodEnum
  {
    defaultSolverMethod,
    richardson,
    chebychev,
    conjugateGradient,
      cg=conjugateGradient,           // cg= short PETSc name
    biConjugateGradient,
      bicg=biConjugateGradient,
    biConjugateGradientSquared,
    conjugateGradientSquared,
      cgs=conjugateGradientSquared,
    biConjugateGradientStabilized,    
      bcgs=biConjugateGradientStabilized,
    generalizedMinimalResidual,
      gmres=generalizedMinimalResidual,
    transposeFreeQuasiMinimalResidual, 
      tfqmr=transposeFreeQuasiMinimalResidual,
    transposeFreeQuasiMinimalResidual2,         // tcqmr Tony Chan's version
      tcqmr=transposeFreeQuasiMinimalResidual2,
    conjugateResidual,
      cr=conjugateResidual,
    leastSquares,
      lsqr=leastSquares,
    preonly,
    qcg                     // minimize a quadratic function
  };
  
  enum PreconditionerEnum
  {
    defaultPreconditioner,
    noPreconditioner,
    jacobiPreconditioner,
    sorPreconditioner,
    luPreconditioner,
    shellPreconditioner,
    DHILUPreconditioner,
    blockJacobiPreconditioner,
    multigridPreconditioner,
    eisenstatPreconditioner,
    incompleteCholeskyPreconditioner,
    incompleteLUPreconditioner,
    additiveSchwarzPreconditioner,
    slesPreconditioner,
    compositePreconditioner,
    redundantPreconditioner,
    diagonalPreconditioner,
    ssorPreconditioner
  };
  
  enum MatrixOrderingEnum
  {
    defaultMatrixOrdering,
    naturalOrdering,
    nestedDisectionOrdering,
    oneWayDisectionOrdering,
    reverseCuthillMcKeeOrdering,
    quotientMinimumDegreeOrdering,
    rowlengthOrdering
   };


  OgesParameters();
  ~OgesParameters();
  OgesParameters& operator=(const OgesParameters& x);
  
  aString getSolverName() const;                      // return the name (composite of solver, preconditioner,...)
  aString getSolverTypeName(SolverEnum solverType = defaultSolver) const;         
  aString getSolverMethodName(SolverMethodEnum solverMethod = defaultSolverMethod ) const;         
  aString getPreconditionerName(PreconditionerEnum preconditioner = defaultPreconditioner) const; 
  aString getMatrixOrderingName(MatrixOrderingEnum matrixOrdering = defaultMatrixOrdering ) const;         

  int setParameters( const Oges & oges);       // set all parameters equal to those values found in oges.
  int update( GenericGraphicsInterface & gi, CompositeGrid & cg ); // update parameters interactively

  // ----------------Functions to set parameters -----------------------------
  int set( SolverEnum option );
  int set( SolverMethodEnum option );
  int set( MatrixOrderingEnum option );
  int set( PreconditionerEnum option );
	   
  int set( OptionEnum option, int value=0 );
  int set( OptionEnum option, float value );
  int set( OptionEnum option, double value );
	   		      
  int get( OptionEnum option, int & value ) const;
  int get( OptionEnum option, real & value ) const;

  OgmgParameters* getOgmgParameters() const;
  
  int get( const GenericDataBase & dir, const aString & name);
  int put( GenericDataBase & dir, const aString & name) const;
  
  int isAvailable( SolverEnum solverType );
  
  // print out current values of parameters
  int display(FILE *file = stdout);

 protected:

  int set( OptionEnum option, int value, real rvalue );
  int get( OptionEnum option, int & value, real & rvalue ) const;


  SolverEnum solver;
  aString solverName;
  
  SolverMethodEnum solverMethod;
  PreconditionerEnum preconditioner;
  MatrixOrderingEnum matrixOrdering;
  
  real relativeTolerance;          // relative tolerance. if <=0. routine should choose a value.
  real absoluteTolerance;
  real maximumAllowableIncreaseInResidual; // stop iterations if residual increases greater than this value.

  bool compatibilityConstraint;
  int gmresRestartLength;
  int numberOfIncompleteLULevels;
  real incompleteLUExpectedFill;
  int minimumNumberOfIterations;
  int maximumNumberOfIterations;
  int solveForTranspose;
  int rescaleRowNorms;

  real matrixCutoff;        // epsz
  bool fixupRightHandSide; // zero out rhs at interp., extrap and periodic Points?

  real zeroRatio;         // zratio
  real fillinRatio;       // fratio
  real fillinRatio2;      // fratio2
  real harwellPivotingTolerance;  // tolerance for harwell pivoting
  real nullVectorScaling;

  int preconditionBoundary;
  int preconditionRightHandSide;
  int maximumInterpolationWidth;
  int iterativeImprovement; 
  
  real sorOmega;                              // omega


  // parameters for experimental DH preconditioner, used with PETSc **pf**
  real incompleteLUDropTolerance;          // =dt for ILUT(dt), ILUTK(dt,k) (from DH preconditioners)
  real incompleteLUSparseACoefficient;     // =sparseA for sparsifying matrix prior to ILU (from DH)
  real incompleteLUSparseFCoefficient;     // =sparseF for sparsifying factorization after ILU (from DH)
  int  incompleteLUTypeInDH;               // =ilu_type in DH preconditioners: 1=iluk with sparsify

  // parameters for memory management: These are all false by default.
  bool keepCoefficientGridFunction; // keep a reference to the user's coeff grid function
  bool keepSparseMatrix;            // keep ia,ja,a sparse matrix even it not needed by the solver
  bool removeSolutionAndRHSVector;    // de-allocate sol and rhs vector after every solve
  bool removeSparseMatrixFactorization; // de-allocate sparse matrix factorization after solving.

  OgmgParameters *ogmgParameters;

  friend class Oges;
  friend class EquationSolver;
  friend class PETScEquationSolver;
  friend class YaleEquationSolver;
  friend class HarwellEquationSolver;
  friend class SlapEquationSolver;
  
};

#endif
