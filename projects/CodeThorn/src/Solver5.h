#ifndef SOLVER5_H
#define SOLVER5_H


// CodeThorn includes
#include "Solver.h"

namespace CodeThorn {

/*! 
  * \author Marc Jasper
  * \date 2017.
  * \brief Solver 5 (default solver)
 */
  class Solver5 : public Solver {
  public:
    Solver5();
    void run();
    int getId();
    bool createsTransitionSystem();
    bool checksAssertions();
    
    static void initDiagnostics();
  private:
    static Sawyer::Message::Facility logger;
    static bool _diagnosticsInitialized;
  };

} // end of namespace CodeThorn

#endif
