#include "sage3basic.h"

#include "DominatorTree.h"

// DQ (12/31/2005): This is OK if not declared in a header file
using namespace std;

// DQ (3/21/2006): Added namespace (see comment in header file)
using namespace DominatorTreesAndDominanceFrontiers;

DominatorTree::DominatorTree(SgNode * head, Direction d) {
  _dir = d;
  if (_dir == PRE) {
    _iddir = ControlFlowGraph::FORWARD;
  } else {
    _iddir = ControlFlowGraph::BACKWARD;
  }
  _buildCFG(head);
  ROSE_ASSERT(_cfg != NULL);
  _setupStructures();
  _findDominators();
}

void DominatorTree::_buildCFG(SgNode * head) {
  _cfg = new ControlFlowGraph(head);
}

void DominatorTree::_setupStructures() {
  _size = _cfg->getSize();
  doms = new int[_size];
  for (int i = 0; i < _size; i++) {
    doms[i] = -1;
  }
  //initialize the start node:
  doms[0] = 0;
}

void DominatorTree::_findDominators() {
  bool changed = true;

  while(changed) {
    changed = false;
    for (int i = 1; i < _size; i++) {
      changed |= _processNode(i);
    }
  }
}

int DominatorTree::_intersect(int f1, int f2) {
  while (f1 != f2) {
    //If we're finding dominators, we want the *smallest* common number.
    //If we're finding post-dominators, we want the *largest*
    while (f1 > f2) {
      f1 = doms[f1];
    }
    while (f2 > f1) {
      f2 = doms[f2];
    }
  }
  return f1;
}

void DominatorTree::printCFG() {
  printf("CFG:\n\n");
  _cfg->printGraph();
}
  
void DominatorTree::printDominators() {

  printf("Dominators:\n");
  for (int i = 0; i < _size; i++) {
    printf("Stmt: %d\tDom: %d\n", i, doms[i]);
  }
  printf("\n");
}

bool DominatorTree::_processNode(int i) {
  
  //find a predecessor of the current node which hasn't been processed
  //if calculating post-dominators, find a successor
  ControlNode * curr = _cfg->getNode(i, _iddir); //retrieve current node
  set<SimpleDirectedGraphNode *> next_nodes;
  switch (_dir) {
  case PRE:
    next_nodes = curr->getPredecessors();
    break;
  case POST:
    next_nodes = curr->getSuccessors();
    break;
  default:
    ROSE_ASSERT(false);
  }
  set<SimpleDirectedGraphNode *>::iterator node_iter = next_nodes.begin();
  int newDom;
  do { //iterate until we find a processed node
    newDom = dynamic_cast<ControlNode *>(*node_iter)->getID(_iddir);
    node_iter++;
  } while ((doms[newDom] == -1) && (node_iter != next_nodes.end()));
  
  //Intersect the doms of all preds (succs) to find current imm. dom. (post dom)
  for(node_iter = next_nodes.begin(); node_iter != next_nodes.end(); node_iter++) {
    int currDom = dynamic_cast<ControlNode *>(*node_iter)->getID(_iddir);
    if (doms[currDom] != -1) {//if it's been processed, intersect it with newDom
      newDom = _intersect(newDom, currDom);
    }
  }
      
  //Determine if things have changed
  if (doms[i] != newDom) {
    doms[i] = newDom;
    return true;
  }

  //if we get here, we didn't change the dominator
  return false;
}
