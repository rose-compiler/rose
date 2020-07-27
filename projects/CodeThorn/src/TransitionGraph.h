#ifndef TRANSITION_GRAPH
#define TRANSITION_GRAPH

#include "EState.h"

namespace CodeThorn {
  /*! 
   * \author Markus Schordan
   * \date 2012.
   */
  class Transition {
  public:
    Transition();
    Transition(const EState* source,Edge edge, const EState* target);
    const EState* source; // source node
    Edge edge;
    const EState* target; // target node
    string toString(CodeThorn::VariableIdMapping* variableIdMapping=0) const;
    size_t memorySize() const;
  private:
  };
  
  /*! 
   * \author Markus Schordan
   * \date 2012.
   */
  class TransitionHashFun {
  public:
    TransitionHashFun();
    long operator()(Transition* s) const;
  private:
  };
  
  class TransitionEqualToPred {
  public:
    TransitionEqualToPred();
    bool operator()(Transition* t1, Transition* t2) const;
  private:
  };
  
  bool operator==(const Transition& t1, const Transition& t2);
  bool operator!=(const Transition& t1, const Transition& t2);
  bool operator<(const Transition& t1, const Transition& t2);
  
  typedef std::set<const Transition*> TransitionPtrSet;
  typedef std::set<const EState*> EStatePtrSet;
  
  /*! 
   * \author Markus Schordan
   * \date 2012.
   */
  class Analyzer;
  class TransitionGraph : public HSetMaintainer<Transition,TransitionHashFun,TransitionEqualToPred> {
  public:
    typedef std::set<const Transition*> TransitionPtrSet;
    TransitionGraph();
    void setModeLTLDriven(bool mode) { _modeLTLDriven=mode; }
    bool getModeLTLDriven() { return _modeLTLDriven; }
    EStatePtrSet estateSetOfLabel(Label lab);
    EStatePtrSet estateSet();
    long numberOfObservableStates(bool includeIn=true, bool includeOut=true, bool includeErr=true);
    void add(Transition trans);
    string toString(VariableIdMapping* variableIdMapping=0) const;
    LabelSet labelSetOfIoOperations(InputOutput::OpType op);
    Label getStartLabel() { assert(_startLabel!=Label()); return _startLabel; }
    void setStartLabel(Label lab) { _startLabel=lab; }
    // this allows to deal with multiple start transitions (must share same start state)
    const EState* getStartEState();
    void setStartEState(const EState* estate);
    Transition getStartTransition();

    void erase(TransitionGraph::iterator transiter);
    void erase(const Transition trans);

    //! deprecated
    void reduceEStates(std::set<const EState*> toReduce);
    void reduceEState(const EState* estate);
    void reduceEState2(const EState* estate); // used for semantic folding
    void reduceEStates3(std::function<bool(const EState*)> predicate); // used for semantic folding
    TransitionPtrSet inEdges(const EState* estate);
    TransitionPtrSet outEdges(const EState* estate);
    EStatePtrSet pred(const EState* estate);
    EStatePtrSet succ(const EState* estate);
    bool checkConsistency();
    const Transition* hasSelfEdge(const EState* estate);
    // deletes EState and *deletes* all ingoing and outgoing transitions
    void eliminateEState(const EState* estate);
    int eliminateBackEdges();
    void determineBackEdges(const EState* state, std::set<const EState*>& visited, TransitionPtrSet& tpSet);
    void setIsPrecise(bool v);
    void setIsComplete(bool v);
    bool isPrecise();
    bool isComplete();
    void setForceQuitExploration(bool v);
    size_t memorySize() const;
    void setAnalyzer(Analyzer* analyzer) {
      _analyzer=analyzer;
    }
    Analyzer* getAnalyzer() {
      return _analyzer;
    }
    // prints #transitions and details about states on stdout
    void printStgSize(std::string optionalComment);
    // generates info about #transitions and details about states in CSV format
    void csvToStream(std::stringstream& csvStream);
  private:
    Label _startLabel;
    int _numberOfNodes; // not used yet
    std::map<const EState*,TransitionPtrSet > _inEdges;
    std::map<const EState*,TransitionPtrSet > _outEdges;
    std::set<const EState*> _recomputedestateSet;
    bool _preciseSTG;
    bool _completeSTG;
    bool _modeLTLDriven;
    bool _forceQuitExploration;

    // only used by ltl-driven mode in function succ
    Analyzer* _analyzer = nullptr;
    // only used by ltl-driven mode in function succ
    const EState* _startEState = nullptr;
  };
}
#endif
