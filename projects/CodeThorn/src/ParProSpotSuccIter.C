#include "sage3basic.h"
#include "ParProSpotSuccIter.h"

using namespace CodeThorn;
using namespace std;

ParProSpotSuccIter::ParProSpotSuccIter(ParProTransitionGraph& tg, const ParProEState& state, 
				       const boost::unordered_map<string, int>& p2d) 
                                       : stg(tg), propName2DictNum(p2d) {
  _outEdges = stg.succ(&state);
}

void ParProSpotSuccIter::first() {
  iter = _outEdges.begin();
}

void ParProSpotSuccIter::next() {
  ROSE_ASSERT(!done());
  iter++;
}

bool ParProSpotSuccIter::done() const {
  return iter == _outEdges.end();
}

spot::state* ParProSpotSuccIter::current_state() const {
  ROSE_ASSERT(!done());
  ParProSpotState* nextState = new ParProSpotState( *iter->target );  
  return nextState;
}

bdd ParProSpotSuccIter::current_condition() const {
  assert(!done()); 
  return generateSpotTransition(*iter);
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
