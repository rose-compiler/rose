#ifndef SOLVER4_H
#define SOLVER4_H


// CodeThorn includes
#include "Solver.h"

namespace CodeThorn {

/*! 
  * \author Marc Jasper
  * \date 2017.
  * \brief Solver 4
 */
  class Solver4 : public Solver {
  public:
    Solver4();
    void run();
    int getId();

  private:
    void initDiagnostics();

    static Sawyer::Message::Facility logger;
    static bool _diagnosticsInitialized;
  };

} // end of namespace CodeThorn

#endif
