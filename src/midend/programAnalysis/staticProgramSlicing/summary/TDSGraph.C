#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include "rose.h"

#include "SummaryGraph.h"

TDSGraph::TDSGraph(InterproceduralInfo * ii) : _inter(ii) {

  //create a new node for each FORMAL argument in _inter
  //go through formal-in params
  map<SgInitializedName *, DependenceNode *>::iterator i;
  for (i = _inter->procedureEntry.formal_in.begin(); i != _inter->procedureEntry.formal_in.end(); i++) {
    DependenceNode * node = i->second;
    createNode(node);
  }
  //go through formal-out params
  for (i = _inter->procedureEntry.formal_out.begin(); i != _inter->procedureEntry.formal_out.end(); i++) {
    DependenceNode * node = i->second;
    createNode(node);
  }
  //add the formal_return
  createNode(_inter->procedureEntry.formal_return);

}

bool TDSGraph::hasUnmarked() {
  return !unmarked.empty();
}

DependenceGraph::Edge TDSGraph::popUnmarked() {
  ROSE_ASSERT(hasUnmarked());
  set<Edge>::iterator i = unmarked.begin();
  Edge retval = *i;
  unmarked.erase(i);
  return retval;
}

void TDSGraph::pushUnmarked(Edge e) {
  unmarked.insert(e);
}
