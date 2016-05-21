// Author: Marc Jasper, 2016.

#include "ParProTransitionGraph.h"

using namespace SPRAY;
using namespace std;

bool SPRAY::operator==(const ParProTransition& t1, const ParProTransition& t2) {
  // TODO: decide how edges from different CFGs are handled. Currently ignored
  //  return (t1.source==t2.source && t1.edge==t2.edge && t1.target==t2.target);
  return (t1.source==t2.source && t1.target==t2.target);
}

bool SPRAY::operator!=(const ParProTransition& t1, const ParProTransition& t2) {
  return !(t1==t2);
}

bool SPRAY::operator<(const ParProTransition& t1, const ParProTransition& t2) {
  if (t1.source!=t2.source) {
    return (t1.source<t2.source);
    // TODO: decide how edges from different CFGs are handled. Currently ignored
    //  } else if (t1.edge!=t2.edge) {
    //    return (t1.edge<t2.edge);
    //  }
  }
  return (t1.target<t2.target);
}

void ParProTransitionGraph::add(ParProTransition transition) {
  EStateTransitionMap::iterator res = _outEdges.find(transition.source);
  if (res != _outEdges.end()) {
    // transitions with the same start state exist already, add to the adjacency set
    res->second.insert(transition);
  } else {
    ParProTransitions newTransitionSet;
    newTransitionSet.insert(transition);
    _outEdges[transition.source] = newTransitionSet;
  }
}

ParProTransitions ParProTransitionGraph::succ(const ParProEState* source) {
  return _outEdges[source];
}

// TODO: store result and a maintain a flag during "add(...)"
size_t ParProTransitionGraph::size() {
  size_t size = 0;
  for (EStateTransitionMap::iterator i=_outEdges.begin(); i!=_outEdges.end(); i++) {
    size += i->second.size();
  }
  return size;
}
