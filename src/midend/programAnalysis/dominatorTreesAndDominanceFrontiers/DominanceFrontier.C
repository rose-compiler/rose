
#include "rose.h"

#include "DominanceFrontier.h"  

// DQ (12/31/2005): This is OK if not declared in a header file
using namespace std;

// DQ (3/21/2006): Added namespace (see comment in header file)
using namespace DominatorTreesAndDominanceFrontiers;

void DominanceFrontier::_buildFrontier() {

  DominatorTree::Direction domDir = _dt->getDirection();
  ControlFlowGraph * cfg = _dt->getCFG();
  ControlFlowGraph::ID_dir cfgDir = _dt->getCFGDirection();

  for (int i = 0; i < _size; i++) {
    ControlNode * currNode = cfg->getNode(i, cfgDir);
    int nextSize;
    if (domDir == DominatorTree::PRE) {
      nextSize = currNode->numPredecessors();
    } else {
      nextSize = currNode->numSuccessors();
    }
    //If this is not a join point, we continue
    if (nextSize < 2) continue;
    
    //From here on, this is a join point
    set<SimpleDirectedGraphNode *> nextNodes;
    if (domDir == DominatorTree::PRE) {
      nextNodes = currNode->getPredecessors();
    } else {
      nextNodes = currNode->getSuccessors();
    }
    set<SimpleDirectedGraphNode *>::iterator next;
    for (next = nextNodes.begin(); next != nextNodes.end(); next++) {
      ControlNode * runner = dynamic_cast<ControlNode *>(*next);
      ROSE_ASSERT(runner != NULL);
      int runnerID = runner->getID(cfgDir);
      int currID = currNode->getID(cfgDir);
      while (runnerID != _dt->getDom(currID)) {
        _domFrontier[runnerID].insert(currID);
        runnerID = _dt->getDom(runnerID);
      }
    }
  }

}

void DominanceFrontier::printFrontier() {
  printf("DominanceFrontier:\n\n");
  for (int i = 0; i < _size; i++) {
    printf("%d:\n   ", i);
    set<int> frontier = getFrontier(i);
    for (set<int>::iterator j = frontier.begin(); j != frontier.end(); j++) {
      printf("%d ", *j);
    }
    printf("\n");
  }
}
