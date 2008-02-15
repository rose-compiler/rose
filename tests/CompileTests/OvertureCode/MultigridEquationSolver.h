#ifndef MULTIGRID_EQUATION_SOLVER_H
#define MULTIGRID_EQUATION_SOLVER_H

//
//  Multigrid solver for Oges
// 

#include <iostream.h>
#include <math.h>
#include <assert.h>

#include "EquationSolver.h"
#include "Ogmg.h"


class MultigridEquationSolver : public EquationSolver
{
 public:
  MultigridEquationSolver(Oges & oges_);
  virtual ~MultigridEquationSolver();

  virtual int solve(realCompositeGridFunction & u,
		    realCompositeGridFunction & f);

  virtual int printStatistics( FILE *file = stdout ) const;   // output any relevant statistics 

  virtual real sizeOf( FILE *file=NULL ); // return number of bytes allocated 

 protected:

  Ogmg ogmg;
  
};


#endif
