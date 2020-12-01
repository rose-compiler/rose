#ifndef I_O_SOLVER_H
#define I_O_SOLVER_H

#include "Solver.h"

namespace CodeThorn {
  
  class IOAnalyzer;

/*! 
  * \author Marc Jasper
  * \date 2017.
  * \brief Input/output solver interface
 */
  class IOSolver : public Solver {
  public:
    IOSolver();
    IOSolver(CTAnalysis* analyzer);

  protected:
    virtual void setAnalyzer(CTAnalysis* analyzer);

    IOAnalyzer* _analyzer = nullptr;
  };

} // end of namespace CodeThorn

#endif
