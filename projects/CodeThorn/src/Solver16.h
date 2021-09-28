#ifndef SOLVER16_H
#define SOLVER16_H


// CodeThorn includes
#include "Solver.h"

namespace CodeThorn {

  class EState;
  
  class Solver16 : public Solver {
  public:
    Solver16();
    void run();
    int getId();

    static void initDiagnostics();
  private:
    void initializeSummaryStatesFromWorkList();
    // add Edge {(currentEStatePtr,e,NewEStatePtr)} to STS
    // if currentEStatePtr!=currentEStatePtr) then also add 
    //     (currentEStatePtr,e,NewEStatePtr)} where e'=(currentEStatePtr0,annot(e),NewStatePtr);
    // this represents the effect of merging states also in the STS (without introducing new merge states)
    void recordTransition(const EState* currentEStatePtr0,const EState* currentEStatePtr,Edge e, const EState* newEStatePtr);
    static Sawyer::Message::Facility logger;
    static bool _diagnosticsInitialized;
  };

} // end of namespace CodeThorn

#endif
