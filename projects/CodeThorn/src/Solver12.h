#ifndef SOLVER12_H
#define SOLVER12_H


// CodeThorn includes
#include "Solver.h"

namespace CodeThorn {

/*! 
  * \author Marc Jasper
  * \date 2017.
  * \brief Solver 12
 */
  class Solver12 : public Solver {
  public:
    Solver12();
    void run();
    int getId();

  private:
    void initDiagnostics();

    static Sawyer::Message::Facility logger;
    static bool _diagnosticsInitialized;
  };

} // end of namespace CodeThorn

#endif
