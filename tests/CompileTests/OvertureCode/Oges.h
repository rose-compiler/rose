#ifndef OGES_H
#define OGES_H "Oges.h"

#include "Overture.h"          

//===========================================================================
//  Overlapping Grid Equation Solver
//
//===========================================================================

#include "OGFunction.h"
#include "OgesParameters.h"

#include "ListOfIntSerialArray.h"
#include "ListOfFloatSerialArray.h"
#include "ListOfDoubleSerialArray.h"

#ifdef OV_USE_DOUBLE
  typedef ListOfDoubleSerialArray ListOfRealSerialArray;
#else
  typedef ListOfFloatSerialArray ListOfRealSerialArray;
#endif

class GenericGraphicsInterface;
class EquationSolver;

class Oges 
{
  public:

  Oges();	
  Oges( CompositeGrid & cg );
  Oges( MappedGrid & mg );
  Oges(const Oges & X);
  Oges & operator=( const Oges & x );
  virtual ~Oges();	
   
  void determineErrors(realCompositeGridFunction & u,  // is this needed?
                       OGFunction & exactSolution,
		       int & printOptions );

  aString getErrorMessage( const int errorNumber );    // is this needed?

  int get( OgesParameters::OptionEnum option, int & value ) const;
  int get( OgesParameters::OptionEnum option, real & value ) const;

  int getCompatibilityConstraint() const;
  int getNumberOfIterations();
  real getMaximumResidual() const;

  int initialize( );
  bool isSolverIterative() const;  // TRUE if the solver chosen is an iterative method
  bool canSolveInPlace() const;

  // output any relevant statistics 
  int printStatistics( FILE *file = stdout ) const;

  // ----------------Functions to set parameters -----------------------------
  int set( OgesParameters::SolverEnum option );
  int set( OgesParameters::SolverMethodEnum option );
  int set( OgesParameters::MatrixOrderingEnum option );
  int set( OgesParameters::PreconditionerEnum option );
	   
  int set( OgesParameters::OptionEnum option, int value=0 );
  int set( OgesParameters::OptionEnum option, float value );
  int set( OgesParameters::OptionEnum option, double value );
	   		      
  int setOgesParameters( const OgesParameters & opar );
  
  virtual real sizeOf( FILE *file=NULL ) const ; // return number of bytes allocated by Oges, print info to a file
  
  // data base IO
  virtual int get( const GenericDataBase & dir, const aString & name);    // get from a database file
  virtual int put( GenericDataBase & dir, const aString & name) const;    // put to a database file

  int writeMatrixToFile( aString fileName );
  int writeMatrixGridInformationToFile( aString fileName );
  int writePetscMatrixToFile( aString filename,
			      realCompositeGridFunction & u,
			      realCompositeGridFunction & f);

  void reference(const Oges &); 

  int  setCoefficientArray( realCompositeGridFunction & coeff );
  int  setCoefficientArray( realMappedGridFunction & coeff );

  // only solve the equations on some grids:
  int setGridsToUse( const IntegerArray & gridsToUse ); 

  // supply command line arguments used by some solvers such as PETSc
  int setCommandLineArguments( int argc, char **argv );

  void setGrid( CompositeGrid & cg );
  void setGrid( MappedGrid & mg );

  int solve( realCompositeGridFunction & u, realCompositeGridFunction & f ); 
  int solve( realMappedGridFunction & u, realMappedGridFunction & f ); 

  int updateToMatchGrid( CompositeGrid & cg );
  int updateToMatchGrid( MappedGrid & mg );
  
  int update( GenericGraphicsInterface & gi, CompositeGrid & cg ); // update parameters interactively



 public:
  // Here are the data and functions used by the EquationSolver classes

  int ndia;
  int ndja;
  int nda;

  RealArray sol,rhs;  // solution and rhs stored as a single long vector
  IntegerArray ia,ja; // rows and columns stored in a sparse format
  RealArray a;        // matrix entries stored in a sparse format

  enum SparseStorageFormatEnum
  {
    uncompressed,
    compressedRow,
    other
  } sparseStorageFormat;
  
  int buildEquationSolvers(OgesParameters::SolverEnum solver);  // call this to build a sparse solver.

  int formMatrix(int & numberOfEquations, int & numberOfNonzeros,
                 SparseStorageFormatEnum storageFormat,
		 bool allocateSpace = TRUE,
		 bool factorMatrixInPlace = FALSE );

  int formRhsAndSolutionVectors(realCompositeGridFunction & u, 
				realCompositeGridFunction & f );
  int storeSolutionIntoGridFunction();


  
 public:

  CompositeGrid cg;
  OgesParameters parameters;  // This object holds parameters for Oges

  realCompositeGridFunction coeff;    // holds discrete coefficients

  intArray *classify;                 // holds classify arrays if we destroy the coeff grid function;

  realCompositeGridFunction uLinearized;       // linearized u for nonlinear problems

  ListOfRealSerialArray ul,fl; // These are referenced to the user's u and f

  // extra equation info, such as a compatibility constraint
  int numberOfExtraEquations;          
  IntegerArray extraEquationNumber;
  realCompositeGridFunction *coefficientsOfDenseExtraEquations;
  realCompositeGridFunction rightNullVector;   // right null vector used as a compatibility constraint

  int solverJob;          // job
  int initialized;        // logical
  bool shouldBeInitialized;   // TRUE is initialize() should be called
  int numberOfGrids;  // local copy
  int numberOfDimensions;
  int numberOfComponents; // same as in coeff[0].sparse->numberOfComponents

  int refactor;
  int reorder;
  int evaluateJacobian;

  // int matrixHasChanged;  // true if the matrix has changed. 
  
  int numberOfEquations;       // neq
  int numberOfNonzerosBound;   // nqs : bound on number of nonzeros
  int numberOfNonzeros;        // nze

    
  int preconditionBoundary;
  int preconditionRightHandSide;

  // Convert an Equation Number to a point on a grid (Inverse of equationNo)
  void equationToIndex( const int eqnNo0, int & n, int & i1, int & i2, int & i3, int & grid );
  

  int numberOfIterations; // number of iterations used by iterative solvers

  int argc;    // copy of command line arguments for PETSc
  char **argv;
  
  enum EquationSolverEnum
  {
    maximumNumberOfEquationSolvers=10
  };
  // Here is where we keep the objects that interface to various solvers: yale, harwell, slap, petsc..
  EquationSolver *equationSolver[maximumNumberOfEquationSolvers];
  
// -----------Here are protected member functions-------------------
 protected:

  IntegerArray gridEquationBase;  // gridEquationBase(grid) = first eqn number on a grid.

  IntegerArray gridsToUse;    // only solve on this list of grids.

  void setup();
  void findExtraEquations();
  void makeRightNullVector();
  void generateMatrixError( const int nda, const int ieqn );
  void generateMatrix( int & errorNumber );

  void privateUpdateToMatchGrid();

  // --------Utility functions:-------------
  
  inline int arraySize(const int grid, const int axis );
  inline int arrayDims(const int grid, const int side, const int axis );

  // Return the equation number for given indices
  inline int equationNo( const int n, const int i1, const int i2, const int i3, const int grid );
  
  IntegerDistributedArray equationNo(const int n, const Index & I1, const Index & I2, const Index & I3, 
		      const int grid );

 public:
  static int debug;

 public:

  int printObsoleteMessage(const aString & routineName, int option =0 );

// ************************************************************************************************
// ----------all the remaining stuff in the class is obsolete-----------------------------------------
// ************************************************************************************************

 void setCompositeGrid( CompositeGrid & cg );

 enum coefficientTypes // coefficients can be supplied in continous or discrete form
 {
   continuous=0,
   discrete=1
 };

 public:
  // enumerators for available solvers 

   enum solvers
   {
     yale=1,
     harwell=2,
     bcg=3,
     sor=4,
     SLAP,
     PETSc
   };

   enum conjugateGradientTypes
   {
     biConjugateGradient=0,
     biConjugateGradientSquared=1,
     GMRes=2,
     CGStab=3
   };

   enum conjugateGradientPreconditioners
   {
     none=0,
     diagonal=1,
     incompleteLU=2,
     SSOR=3
   };


  void setConjugateGradientType( const conjugateGradientTypes conjugateGradientType );
  void setConjugateGradientPreconditioner( 
     const conjugateGradientPreconditioners conjugateGradientPreconditioner);
  void setConjugateGradientNumberOfIterations( const int conjugateGradientNumberOfIterations);  
  void setConjugateGradientNumberOfSaveVectors( 
     const int conjugateGradientNumberOfSaveVectors ); 
   void setConjugateGradientTolerance( const real conjugateGradientTolerance ); 
   void setCompatibilityConstraint( const bool trueOrFalse ); 
   void setEvaluateJacobian( const int EvaluateJacobian );
   void setFillinRatio( const real fillinRatio );
   void setFillinRatio2( const real fillinRatio2 );
   void setFixupRightHandSide( const bool trueOrFalse );
   void setHarwellTolerance( const real harwellTolerance);  // tolerance for harwell pivoting
   void setIterativeImprovement( const int trueOrFalse );  

  void setNumberOfComponents( const int numberOfComponents );  // **** this is needed or get from SparseRep!

  void setNullVectorScaling(const real & scale );
  void setMatrixCutoff( const real matrixCutoff );

  void setOrderOfAccuracy( const int order ); // **** this is needed or get from SparseRep!

  void setPreconditionBoundary( const int preconditionBoundary );
  void setPreconditionRightHandSide( const int preconditionRightHandSide );
  void setRefactor( const int refactor );
  void setReorder( const int reorder );
  void setSolverJob( const int solverJob );
  void setSolverType( const solvers solverType );
  void setSorNumberOfIterations( const int sorNumberOfIterations );  
  void setSorTolerance( const real sorTolerance );
  void setSorOmega( const real sorOmega ); 
//  void setSparseFormat( const int sparseFormat );
  void setTranspose( const int transpose );
  void setZeroRatio( const real zeroRatio );

  void setCoefficientType( const coefficientTypes coefficientType );


//  solvers solverType;         // solver
  coefficientTypes coefficientType;

  real actualZeroRatio;         
  real actualFillinRatio;       
  real actualFillinRatio2;      
  real maximumResidual;  
  
// --------------------------------------------------------------------------------
// --------------------------------------------------------------------------------
// --------------------------------------------------------------------------------


};


inline int Oges::
arraySize(const int grid, const int axis )
{
  return cg[grid].dimension(End,axis)-cg[grid].dimension(Start,axis)+1;
}

inline int Oges::
arrayDims(const int grid, const int side, const int axis )
{
  return cg[grid].dimension(side,axis);
}


inline int Oges::
equationNo( const int n, const int i1, const int i2, const int i3, 
	    const int grid )
//=============================================================================
  // Return the equation number for given indices
  //  n : component number ( n=0,1,..,numberOfComponents-1 )
  //  i1,i2,i3 : grid indices
  //  grid : component grid number (grid=0,1,2..,numberOfCompoentGrids-1)   
  //=============================================================================
{
  return n+1+   numberOfComponents*(i1-cg[grid].dimension(Start,axis1)+
	(cg[grid].dimension(End,axis1)-cg[grid].dimension(Start,axis1)+1)*(i2-cg[grid].dimension(Start,axis2)+
        (cg[grid].dimension(End,axis2)-cg[grid].dimension(Start,axis2)+1)*(i3-cg[grid].dimension(Start,axis3)
                                                                           ))) + gridEquationBase(grid);
}


#endif // OGES_H

