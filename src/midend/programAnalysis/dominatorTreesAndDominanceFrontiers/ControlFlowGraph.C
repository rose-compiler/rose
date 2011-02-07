
#include "rose.h"

#include "ControlFlowGraph.h"

// DQ (12/31/2005): This is OK if not declared in a header file
using namespace std;

// DQ (3/21/2006): Added namespace (see comment in header file)
using namespace DominatorTreesAndDominanceFrontiers;


ControlFlowGraph::ControlFlowGraph(SgNode * head) :
  _cfg(NULL),
  _numNodes(0),
  _entry(NULL),
  _exit(NULL)
{
  _buildCFGImpl(head);

  _buildCFG();
}

void ControlFlowGraph::createNode(CFGNodeImpl * node) {

  ControlNode * toInsert;

  //If the node is empty, put it in _cfgnodemap, otherwise put its statements in _sgnodemap
  list<AstNodePtr> stmtList = node->GetStmts();
  if (stmtList.empty()) {
    ControlNode * toInsert = new ControlNode;
    _cfgnodemap[node] = toInsert;
    // printf("Empty node!\n");
    addNode(toInsert);
    _numNodes++;
  } else {
    // printf("Adding Statements:\n");
    for (list<AstNodePtr>::iterator i = stmtList.begin(); i != stmtList.end(); i++) {
      SgNode * curr = *i;

      ControlNode * toInsert = new ControlNode(curr);
      _sgnodemap[curr] = toInsert;
      // printf("Added stmt!\n");
      addNode(toInsert);
      _numNodes++;
    }
  }
}

void ControlFlowGraph::_buildCFGImpl(SgNode * head) {
  _cfg = new DefaultCFGImpl();
  AstInterface fa(head);
  ROSE_Analysis::BuildCFG(fa, head, *_cfg);
}

void ControlFlowGraph::_buildCFG() {

  //Go through each CFGNodeImpl in CFGImpl to find the "entry" node
  DefaultCFGImpl::NodeIterator nodes;
  CFGNodeImpl * entry;
  CFGNodeImpl * exit;
  for (nodes = _cfg->GetNodeIterator(); !nodes.ReachEnd(); ++nodes) {
    //    printf("\nNODE!\n\n");
    CFGNodeImpl * node = *nodes;
    if (_cfg->GetPredecessors(node).ReachEnd() &&
        _cfg->GetSuccessors(node).ReachEnd()) {
      //      printf("Skipping broken node\n");
      continue;
    }

    //get incoming edges
    DefaultCFGImpl::NodeIterator preds = _cfg->GetPredecessors(node);
    //determine whether there are no predecessors
    if (preds.ReachEnd()) {
      //      printf("Found entry!\n");
      entry = node;
    }
    //get outgoing edges
    DefaultCFGImpl::NodeIterator succs = _cfg->GetSuccessors(node);
    //determine whether there are no successors
    if (succs.ReachEnd()) {
      //      printf("Found exit!\n");
      exit = node;
    }

    //Add the node itself to the graph
    createNode(node);
  }
 
  //set _entry and _exit appropriately
  if (_cfgnodemap.count(entry))
    _entry = _cfgnodemap[entry];
  else
    _entry = _sgnodemap[entry->GetStmts().front()];

  if (_cfgnodemap.count(exit))
    _exit = _cfgnodemap[exit];
  else
    _exit = _sgnodemap[exit->GetStmts().back()];

  //For each CFGNodeImpl, add the appropriate links in ControlFlowGraph
  for (nodes = _cfg->GetNodeIterator(); !nodes.ReachEnd(); ++nodes) {
    list<AstNodePtr> stmts = (*nodes)->GetStmts();
    if(stmts.empty()) {
      _buildBranches(_cfgnodemap[*nodes], *nodes);
    } else {
      list<AstNodePtr>::iterator curr = stmts.begin();
      list<AstNodePtr>::iterator next = stmts.begin();
      next++;
      while (next != stmts.end()) {
        SgNode * from = *curr;
        SgNode * to = *next;
        addLink(_sgnodemap[from], _sgnodemap[to]);
        curr++;
        next++;
      }
      SgNode * last = *curr;
      _buildBranches(_sgnodemap[last], *nodes);
    }
  }

  //set up the id maps
  _setupIDs(FORWARD);
  _setupIDs(BACKWARD);

  return;
}

void ControlFlowGraph::_buildBranches(ControlNode * from, CFGNodeImpl * curr) {

  //Basic idea: get the successors from curr, for each one either use
  //the node itself to generate the ControlNode, or use the first
  //statement.

  DefaultCFGImpl::NodeIterator succs = _cfg->GetSuccessors(curr);
  for ( ; !succs.ReachEnd(); ++succs) {
    CFGNodeImpl * node = *succs;
    list<AstNodePtr> stmts = node->GetStmts();
    if (stmts.empty()) { //if there are no statements, use the node itself
      addLink(from, _cfgnodemap[node]);
    } else {
      addLink(from, _sgnodemap[stmts.front()]);
    }
  }
}

void ControlFlowGraph::_setupIDs(ID_dir direction) {
  //Do a BFS on the CFG, initializing IDs as we go
  int currid = 0;
  queue<SimpleDirectedGraphNode *> remaining;
  set<SimpleDirectedGraphNode *> seen;
  
  _forIndex = new ControlNode*[_numNodes];
  _backIndex = new ControlNode*[_numNodes];

  if (direction == FORWARD) {
    remaining.push(_entry);
  } else {
    remaining.push(_exit);
  }

  while (!remaining.empty()) {

    ControlNode * curr = dynamic_cast<ControlNode *>(remaining.front());
    remaining.pop();

    if (seen.count(curr)) {//if we've seen it, ignore it
      continue;
    } else {
      seen.insert(curr);
    }

    //assign an id to i
    ROSE_ASSERT(currid < _numNodes);
    if (direction == FORWARD) {
      _forIndex[currid] = curr;
      curr->setID(currid, FORWARD);
    } else {
      _backIndex[currid] = curr;
      curr->setID(currid, BACKWARD);
    }
    currid++;

    //push its neighbors onto the queue
    set<SimpleDirectedGraphNode *> neighbors;
    if (direction == FORWARD) {
      neighbors = curr->getSuccessors();
    } else {
      neighbors = curr->getPredecessors();
    }
    for (set<SimpleDirectedGraphNode *>::iterator i = neighbors.begin(); i != neighbors.end(); i++) {
      remaining.push(*i);
    }
  }
}

void ControlFlowGraph::outputCFGImpl() {
  GraphDotOutput output(*_cfg);
  string fileName = "cfg.dot";
  output.writeToDOTFile(fileName);
}

void ControlFlowGraph::_displayData(SimpleDirectedGraphNode * data, ostream & os) {
  ControlNode * node = dynamic_cast<ControlNode *>(data);
  node->writeOut(os);
}
