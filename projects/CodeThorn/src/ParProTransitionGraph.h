// Author: Marc Jasper, 2016.

#ifndef PAR_PRO_TRANSITION_GRAPH_H
#define PAR_PRO_TRANSITION_GRAPH_H

#include "sage3basic.h"

// SPRAY includes
// #include "Flow.h"
#include "StateRepresentations.h"

// CodeThorn includes


// BOOST includes
#include "boost/lexical_cast.hpp"
#include "boost/unordered_set.hpp"
#include "boost/regex.hpp"
#include "boost/algorithm/string.hpp"
#include "boost/algorithm/string/trim.hpp"
#include "boost/algorithm/string/regex.hpp"

using namespace CodeThorn;

namespace SPRAY {

  class ParProTransition {
  public:
    ParProTransition(const ParProEState* s, Edge e, const ParProEState* t) : source(s), edge(e), target(t) {}

    const ParProEState* source;
    Edge edge;
    const ParProEState* target;
  };
  
  bool operator==(const ParProTransition& t1, const ParProTransition& t2);
  bool operator!=(const ParProTransition& t1, const ParProTransition& t2);
  bool operator<(const ParProTransition& t1, const ParProTransition& t2);

  typedef std::set<ParProTransition> ParProTransitions; // multiple identical edges are not desired in the STG
  typedef boost::unordered_map<const ParProEState*, ParProTransitions> EStateTransitionMap;

  // the state transition graph of a parallel program
  class ParProTransitionGraph {
  public:
    ParProTransitionGraph() : _preciseStg(true), _completeStg(false) {}
    void add(ParProTransition transition);
    void setStartState(const ParProEState* state) { _startState = state; } 
    const ParProEState* getStartState() { return _startState; } 
    ParProTransitions succ(ParProEState* source);
    EStateTransitionMap* getOutEdgesMap() { return &_outEdges; }
    size_t size();
    bool isPrecise() { return _preciseStg; }
    void setIsPrecise(bool p) { _preciseStg = p; }
    bool isComplete() { return _completeStg; }
    void setIsComplete(bool c) { _completeStg = c; }

  private:
    EStateTransitionMap _outEdges;
    const ParProEState* _startState;
    bool _preciseStg;
    bool _completeStg;
  };

} // end of namespace SPRAY

#endif
