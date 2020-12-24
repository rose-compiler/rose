#ifndef SOLVER16_H
#define SOLVER16_H


// CodeThorn includes
#include "Solver.h"

namespace CodeThorn {

/*! 
  * \author Marc Jasper
  * \date 2017.
  * \brief Solver 5 (default solver)
 */
  class Solver16 : public Solver {
  public:
    Solver16();
    void run();
    int getId();

  private:
    void initDiagnostics();
    static Sawyer::Message::Facility logger;
    static bool _diagnosticsInitialized;
  };

} // end of namespace CodeThorn

#endif
