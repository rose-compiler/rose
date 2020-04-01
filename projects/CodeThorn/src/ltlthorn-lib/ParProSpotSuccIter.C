#include "sage3basic.h"
#include "rose_config.h"
#ifdef HAVE_SPOT

#include "ParProSpotSuccIter.h"

using namespace CodeThorn;
using namespace std;

ParProSpotSuccIter::ParProSpotSuccIter(ParProTransitionGraph& tg, const ParProEState& state, 
				       const boost::unordered_map<string, int>& p2d) 
                                      : _source(state), stg(tg), propName2DictNum(p2d) {
  _outEdges = stg.succ(&state);
  if (stg.isComplete() && !stg.isPrecise()) {
    _needToAddSelfLoop = true;
    _upcomingSelfLoop = true;
  } else {
    _needToAddSelfLoop = false;
  }
}

void ParProSpotSuccIter::first() {
  iter = _outEdges.begin();
  if (_needToAddSelfLoop) {
    _upcomingSelfLoop = true;
  }
}

void ParProSpotSuccIter::next() {
  ROSE_ASSERT(!done());
  if (!_needToAddSelfLoop) {
    ++iter;
  } else {
    if (iter != _outEdges.end()) {
      ++iter;
    } else {
      _upcomingSelfLoop = false;
    }
  }
}

bool ParProSpotSuccIter::done() const {
  if (!_needToAddSelfLoop) {
    return iter == _outEdges.end();
  } else {
    return iter == _outEdges.end() && !_upcomingSelfLoop;
  }
}

spot::state* ParProSpotSuccIter::current_state() const {
  ROSE_ASSERT(!done());
  if (!_needToAddSelfLoop) {
    ParProSpotState* nextState = new ParProSpotState( *iter->target );  
    return nextState;
  } else {
    if (iter != _outEdges.end()) {
      ParProSpotState* nextState = new ParProSpotState( *iter->target );  
      return nextState;
    } else {
      return new ParProSpotState(_source); // simulates a self loop
    }
  }
}

bdd ParProSpotSuccIter::current_condition() const {
  assert(!done()); 
  if (!_needToAddSelfLoop) {
    return generateSpotTransition(*iter);
  } else {
    if (iter != _outEdges.end()) {
      return generateSpotTransition(*iter);
    } else {
      Edge edge;
      edge.setAnnotation("over-approximation loop");
      ParProTransition transition(&_source, edge, &_source);
      return generateSpotTransition(transition);
    }
  }
}

string ParProSpotSuccIter::format_current_condition() const {
  assert(!done()); 
  if (!_needToAddSelfLoop) {
    return generateFormattedSpotTransition(*iter);
  } else {
    if (iter != _outEdges.end()) {
      return generateFormattedSpotTransition(*iter);
    } else {
      Edge edge;
      edge.setAnnotation("over-approximation loop");
      ParProTransition transition(&_source, edge, &_source);
      return generateFormattedSpotTransition(transition);
    }
  }
}

bdd ParProSpotSuccIter::current_acceptance_conditions() const {
  assert(!done());
  return bddfalse;
}

bdd ParProSpotSuccIter::generateSpotTransition(const ParProTransition& t) const {
  bdd result = bddtrue;
  string annotation = t.edge.getAnnotation();
  for (String2Int::const_iterator i=propName2DictNum.begin(); i!=propName2DictNum.end(); i++) {
    if (annotation == i->first) {
      result &= bdd_ithvar(i->second);
    } else {
      result &= bdd_nithvar(i->second);
    }
  }
  return result;
}

string ParProSpotSuccIter::generateFormattedSpotTransition(const ParProTransition& t) const {
  stringstream ss;
  string annotation = t.edge.getAnnotation();
  if (propName2DictNum.find(annotation) != propName2DictNum.end()) {
    ss << annotation;
  }
  ss << "\\n";
  bool firstEntry = true;
  for (String2Int::const_iterator i=propName2DictNum.begin(); i!=propName2DictNum.end(); i++) {
    if (!firstEntry && annotation != i->first) {
      ss << ",";
    }
    if (annotation != i->first) {
      ss << "!"<<i->first;
      firstEntry = false;
    } 
  }
  return ss.str();
}

#endif // end of "#ifdef HAVE_SPOT"
