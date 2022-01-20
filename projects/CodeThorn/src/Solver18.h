#ifndef SOLVER18_H
#define SOLVER18_H


// CodeThorn includes
#include "Solver.h"
#include "EState.h"
#include "GeneralPriorityWorkList.h"
#include "Label.h"
#include "CallString.h"

namespace CodeThorn {

  class EState;
  
  class Solver18 : public Solver {
  public:
    Solver18();
    void run();
    int getId();

    static void initDiagnostics();
    // a pass through node has a single in-edge, a single-out edge, and
    // the next node has also a single-in-edge.
    bool isPassThroughLabel(Label lab);
    size_t checkDiff();
    class WorkListEntry {
    public:
      WorkListEntry(Label lab,CallString cs):_label(lab),_callString(cs) {}
      Label label() { return _label; }
      CallString callString() { return _callString; }
    private:
      Label _label;
      CallString _callString;
    };
    bool isReachableLabel(Label lab);
    bool isUnreachableLabel(Label lab);
    void deleteAllStates();
    size_t getNumberOfStates();
  private:
    static Sawyer::Message::Facility logger;
    static bool _diagnosticsInitialized;
    
    GeneralPriorityWorkList<WorkListEntry>* _workList=nullptr;

    void initializeSummaryStatesFromWorkList();
    EStatePtr getSummaryState(CodeThorn::Label lab, CodeThorn::CallString cs);
    void setSummaryState(CodeThorn::Label lab, CallString cs, EStatePtr estate);
    EStatePtr getBottomSummaryState(Label lab, CallString cs);
    typedef std::unordered_map <CallString ,EStatePtr> SummaryCSStateMap;
    std::unordered_map< int, SummaryCSStateMap > _summaryCSStateMapMap;
    // number of active states stored in _summaryCSStateMapMap (computed incrementally by setSummaryState)
    size_t _numberOfStates=0;
  };

} // end of namespace CodeThorn

#endif
