#ifndef SOLVER16_H
#define SOLVER16_H


// CodeThorn includes
#include "Solver.h"

namespace CodeThorn {

  class EState;
  
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
    // add Edge {(currentEStatePtr,e,NewEStatePtr)} to STS
    // if currentEStatePtr!=currentEStatePtr) then also add 
    //     (currentEStatePtr,e,NewEStatePtr)} where e'=(currentEStatePtr0,annot(e),NewStatePtr);
    // this represents the effect of merging states also in the STS (without introducing new merge states)
    void recordTransition(const EState* currentEStatePtr0,const EState* currentEStatePtr,Edge e, const EState* newEStatePtr);
    void initDiagnostics();
    static Sawyer::Message::Facility logger;
    static bool _diagnosticsInitialized;
  };

} // end of namespace CodeThorn

#endif
