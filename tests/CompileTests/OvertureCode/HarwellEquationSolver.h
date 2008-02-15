#ifndef HARWELL_EQUATION_SOLVER_H
#define HARWELL_EQUATION_SOLVER_H

//
//  Harwell solver for Oges
// 

#include <iostream.h>
#include <math.h>
#include <assert.h>

#include "EquationSolver.h"


class HarwellEquationSolver : public EquationSolver
{
 public:
  HarwellEquationSolver(Oges & oges_);
  virtual ~HarwellEquationSolver();

  virtual int solve(realCompositeGridFunction & u,
		    realCompositeGridFunction & f);

 virtual real sizeOf( FILE *file=NULL ); // return number of bytes allocated 

 protected:
  int solve();
  int allocateWorkSpace();
  

  int solverJob;
  RealArray wh;        // for harwell
  IntegerArray ikeep,iwh;

  real fillinRatio, fillinRatio2;
};


#endif
