#ifndef EQUATION_SOLVER_H
#define EQUATION_SOLVER_H

#include "Oges.h"

// base class for equations solvers such as yale, harwell, slap, petsc etc.
class EquationSolver
{
 public:
  EquationSolver(Oges & oges_);
  virtual ~EquationSolver();
  virtual int solve(realCompositeGridFunction & u,
		    realCompositeGridFunction & f)=0;

  virtual int saveBinaryMatrix(aString filename00,
                               realCompositeGridFunction & u,
			       realCompositeGridFunction & f);

  const aString & getName() const;

  virtual real getMaximumResidual(); 

  virtual int printStatistics( FILE *file = stdout ) const;   // output any relevant statistics 

  virtual int allocateMatrix(int,int,int,int);
  virtual int setMatrixElement(int,int,int,real);
  virtual void displayMatrix();

// So far a common data structure is used by all vector types, so there is no need to have these:
//  virtual void setRHSVectorElement(int,real);
//  virtual void setSolVectorElement(int,real);

  virtual real sizeOf( FILE *file=NULL ); // return number of bytes allocated 

 protected:

  Oges & oges;
  OgesParameters & parameters;

  aString name;
  int numberOfEquations;
  int numberOfNonzeros;
  real maximumResidual;   // after solve, this is the maximumResidual (if computed)
  int numberOfIterations; // number of iterations required for the solve.
  
};


#endif
