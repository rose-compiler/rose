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

    class WorkListEntry {
    public:
      WorkListEntry(Label lab,CallString cs):_label(lab),_callString(cs) {}
      Label label() const { return _label; }
      CallString callString() const { return _callString; }
      std::string toString() const { return "("+_label.toString()+","+_callString.toString()+")"; }
    private:
      Label _label;
      CallString _callString;
    };

    // at pass through labels the state does not need to be stored
    // and can be passed through allowing in-place updates of the state.
    bool isPassThroughLabel(Label lab);
    size_t checkDiff();
    bool isReachableLabel(Label lab);
    bool isUnreachableLabel(Label lab);
    void deleteAllStates();
    size_t getNumberOfStates();
    /* checks if at least one state has been computed at this label with the given call string
     * in this case the call string can be used to retrieve the state (otherwise it defaults to the bottom state)
     * this check is used when determining path feasibility in context sensitive analysis
     */
    bool callStringExistsAtLabel(CallString& cs, Label lab);
    void dumpAbstractStateMapMap();
    void setAbstractionConsistencyCheckFlag(bool flag);
    bool getAbstractionConsistencyCheckFlag();
    void setPassThroughOptimizationFlag(bool flag);
    bool getPassThroughOptimizationFlag();
  private:
    static Sawyer::Message::Facility logger;
    static bool _diagnosticsInitialized;
    
    GeneralPriorityWorkList<WorkListEntry>* _workList=nullptr;

    void initializeAbstractStatesFromWorkList();
    EStatePtr getAbstractState(CodeThorn::Label lab, CodeThorn::CallString cs);
    void setAbstractState(CodeThorn::Label lab, CallString cs, EStatePtr estate);
    EStatePtr createBottomAbstractState(Label lab, CallString cs);
    typedef std::unordered_map <CallString ,EStatePtr> AbstractCSStateMap;
    std::unordered_map< int, AbstractCSStateMap > _abstractCSStateMapMap;
    // number of active states stored in _abstractCSStateMapMap (computed incrementally by setAbstractState)
    size_t _numberOfStates=0;
    void printAllocationStats(string text);
    bool _abstractionConsistencyCheckEnabled=false; // slow, only used for debugging
    bool _passThroughOptimizationEnabled=true;
  };

} // end of namespace CodeThorn

#endif
