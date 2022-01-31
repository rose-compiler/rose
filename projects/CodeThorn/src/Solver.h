#ifndef SOLVER_H
#define SOLVER_H

#include "DFSolver2.h"

namespace CodeThorn {

  class CTAnalysis;

  //  class Solver : public DFSolver2 {
  class Solver {

  public:
    Solver();
    Solver(CTAnalysis* analyzer);
    virtual ~Solver();
    virtual void run() = 0;
    virtual int getId() = 0;
    virtual bool createsTransitionSystem(); // default false
    virtual bool checksAssertions(); // default false

    virtual void setAnalyzer(CTAnalysis* analyzer);
  protected:
    static void initDiagnostics(Sawyer::Message::Facility& logger, int solverId);

    CTAnalysis* _analyzer = nullptr;
  };

} // end of namespace CodeThorn

#endif
