#ifndef SOLVER12_H
#define SOLVER12_H


// CodeThorn includes
#include "Solver.h"

namespace CodeThorn {

/*! 
  * \author Marc Jasper
  * \date 2017.
  * \brief Features two work lists ("loop-aware-sync" search mode)
  * \details One work list for the current and one for the next iteration of the outer-most loop of the analyzed program (see ISoLA'16 paper)
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
