#ifndef SOLVER8_H
#define SOLVER8_H


// CodeThorn includes
#include "IOSolver.h"

namespace CodeThorn {

/*! 
  * \author Marc Jasper
  * \date 2017.
  * \brief Analyzes a single trace according to a specific input sequence.
 */
  class Solver8 : public IOSolver {
  public:
    Solver8();
    void run();
    int getId();

  private:
    void initDiagnostics();

    static Sawyer::Message::Facility logger;
    static bool _diagnosticsInitialized;
  };

} // end of namespace CodeThorn

#endif
