#ifndef YALE_EQUATION_SOLVER_H
#define YALE_EQUATION_SOLVER_H

//
//  Yale solver for Oges
// 

#include <iostream.h>
#include <math.h>
#include <assert.h>

#include "EquationSolver.h"


class YaleEquationSolver : public EquationSolver
{
 public:
  YaleEquationSolver(Oges & oges_);
  virtual ~YaleEquationSolver();

  virtual int solve(realCompositeGridFunction & u,
		    realCompositeGridFunction & f);

  virtual real sizeOf( FILE *file=NULL ); // return number of bytes allocated 

 protected:
  int solve();
  int allocateWorkSpace();
  

  IntegerArray perm,iperm;
  int nsp;        
  RealArray rsp;
  int solverJob;
  int yaleExcessWorkSpace;

  real fillinRatio;
};


#endif
