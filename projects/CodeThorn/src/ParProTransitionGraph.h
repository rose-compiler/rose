
#ifndef PAR_PRO_TRANSITION_GRAPH_H
#define PAR_PRO_TRANSITION_GRAPH_H

#include "sage3basic.h"

// CodeThorn includes
#include "Flow.h"
#include "ParProEState.h"
#include "ParallelAutomataGenerator.h"

// CodeThorn includes


// BOOST includes
#include "boost/lexical_cast.hpp"
#include "boost/unordered_set.hpp"
#include "boost/regex.hpp"
#include "boost/algorithm/string.hpp"
#include "boost/algorithm/string/trim.hpp"
#include "boost/algorithm/string/regex.hpp"

using namespace CodeThorn;

namespace CodeThorn {
  /*! 
   * \author Marc Jasper
   * \date 2016.
   */
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

  /*! 
   * \brief State transition graph consisting of ParProEStates.
   * \author Marc Jasper
   * \date 2016.
   * \details Graph is implemented as a map: Each state is mapped to its set of outgoing transitions. 
   Transitions store pointers to their source and target states.
   */
  class ParProTransitionGraph {
  public:
    ParProTransitionGraph() : _preciseStg(true), _completeStg(false) {}
    void deleteStates();
    void add(ParProTransition transition);
    void setStartState(const ParProEState* state) { _startState = state; } 
    const ParProEState* getStartState() { return _startState; } 
    ParProTransitions succ(const ParProEState* source);
    EStateTransitionMap* getOutEdgesMap() { return &_outEdges; }
    size_t size();
    size_t numStates();
    std::set<std::string> getAllAnnotations();
    bool isPrecise() { return _preciseStg; }
    void setIsPrecise(bool p) { _preciseStg = p; }
    bool isComplete() { return _completeStg; }
    void setIsComplete(bool c) { _completeStg = c; }
    Flow* toFlowEnumerateStates(NumberGenerator& numGen);

    // generates a string for visiualization with dot/graphviz.
    std::string toDot();

  private:
    EStateTransitionMap _outEdges;
    const ParProEState* _startState;
    bool _preciseStg;
    bool _completeStg;
  };

} // end of namespace CodeThorn

#endif
