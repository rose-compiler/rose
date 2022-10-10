#ifndef SOLVER11_H
#define SOLVER11_H


// CodeThorn includes
#include "IOSolver.h"

namespace CodeThorn {

/*! 
  * \author Marc Jasper
  * \date 2017.
  * \brief Features a transfer funciton with large-step semantics (LTL-relevant states). 
  * \details Regular interpretation of analyzed program (does not use hybrid mode). Uses the "subSolver". 
 */
  class Solver11 : public IOSolver {
  public:
    Solver11();
    void run();
    int getId();

  private:
    void initDiagnostics();

    static Sawyer::Message::Facility logger;
    static bool _diagnosticsInitialized;
  };

} // end of namespace CodeThorn

#endif
