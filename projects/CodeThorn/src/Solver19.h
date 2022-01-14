#ifndef SOLVER19_H
#define SOLVER19_H


// CodeThorn includes
#include "Solver.h"
#include "EState.h"
#include "GeneralPriorityWorkList.h"
#include "Label.h"
#include "CallString.h"
#include "Flow.h" // class Edge

namespace CodeThorn {

  class EState;
  
  class Solver19 : public Solver {
  public:
    Solver19();
    void run();
    int getId();

    static void initDiagnostics();
    // a pass through node has a single in-edge, a single-out edge, and
    // the next node has also a single-in-edge.
    bool isPassThroughLabel(Label lab);
    class WorkListEntry {
    public:
    WorkListEntry(Edge edge,CallString cs):_edge(edge),_callString(cs) {}
      Label label() { return _edge.source(); } // mapped source or target defines order in WL
      CallString callString() { return _callString; }
    private:
      Edge _edge;
      CallString _callString;
    };
  private:
    void initializeSummaryStatesFromWorkList();
    // add Edge {(currentEStatePtr,e,NewEStatePtr)} to STS
    // if currentEStatePtr!=currentEStatePtr) then also add 
    //     (currentEStatePtr,e,NewEStatePtr)} where e'=(currentEStatePtr0,annot(e),NewStatePtr);
    // this represents the effect of merging states also in the STS (without introducing new merge states)
    static Sawyer::Message::Facility logger;
    static bool _diagnosticsInitialized;
    GeneralPriorityWorkList<WorkListEntry>* _workList=nullptr;
  };

} // end of namespace CodeThorn

#endif
