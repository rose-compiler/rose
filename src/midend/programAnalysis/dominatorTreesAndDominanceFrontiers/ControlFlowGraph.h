#ifndef _CONTROLFLOWGRAPH_H_
#define _CONTROLFLOWGRAPH_H_

#include "SimpleDirectedGraph.h"
#include "CFGImpl.h"
#include "GraphDotOutput.h"
#include "virtualCFG.h"

#include <map>
#include <queue>
#include <set>

// DQ (12/30/2005): This is a Bad Bad thing to do (I can explain)
// it hides names in the global namespace and causes errors in 
// otherwise valid and useful code. Where it is needed it should
// appear only in *.C files (and only ones not included for template 
// instantiation reasons) else they effect user who use ROSE unexpectedly.
// using namespace std;


// DQ (3/21/2006): Namespace introduced to hide redundent use of 
// ControlFlowGraph class also found in:
//      src/midend/programTransformation/partialRedundancyElimination
namespace DominatorTreesAndDominanceFrontiers {

class ControlNode;

/*! \class ControlFlowGraph

  This class constructs a control flow graph for a given function
  definition. It builds off of information provided by CFGImpl.h, and
  adds some additional bookkeeping information (such as entry and exit
  points, as well as having a separate node for each statement in the
  graph).

  This class extends SimpleDirectedGraph templated on ControlNode

*/

class ControlFlowGraph : public SimpleDirectedGraph {

public:

  /*! \brief determines the ordering of id numbers

  Certain uses of the CFG rely on a specific numbering scheme for the
  nodes of the CFG. The nodes can either be numbered in
  breadth-first-search order rooted at the entry node, and following
  edges forward, or in breadth-first-search order rooted at the exit
  node and following edges backward.
  */
  enum ID_dir 
  {
    FORWARD,
    BACKWARD
  };

  //! The constructor for ControlFlowGraph. Builds a CFG rooted at head
  ControlFlowGraph(SgNode * head);
  
  //! from a given CFGImpl node, create one (or more) ControlNodes
  void createNode(CFGNodeImpl * node);

  //! return the number of nodes in the CFG
  int getSize() {return _numNodes;}

  //! given a node id (and which numbering scheme to use), return the appropriate control node
  ControlNode * getNode(int id, ID_dir dir) {return (dir == FORWARD)?_forIndex[id]:_backIndex[id];}

  //! dump the contents of the original CFGImpl to a dot file
  void outputCFGImpl();

private:

  void _buildCFGImpl(SgNode * head);

  void _buildCFG();
  void _buildBranches(ControlNode * from, CFGNodeImpl * curr);
  void _setupIDs(ID_dir);

  virtual void _displayData(SimpleDirectedGraphNode * data, std::ostream & os);

  DefaultCFGImpl * _cfg;

  int _numNodes;
  ControlNode * _entry;
  ControlNode * _exit;

  //! Map from CFGImpl nodes to our ControlNodes (used for empty CFGNodeImpls)
  std::map<CFGNodeImpl *, ControlNode *> _cfgnodemap;
  //! Map from SgNodes to ControlNodes (used for ControlNodes with statements)
  std::map<SgNode *, ControlNode *> _sgnodemap;
  
  //! Array matches IDs to ControlNodes using forward ids
  ControlNode ** _forIndex;
  //! Array matches IDs to ControlNodes using backward ids
  ControlNode ** _backIndex;
};

/*! \class ControlNode

  This class is a node in the CFG. It can either be an SgNode or it
  can be a special entry or exit node (since there are some empty
  blocks in the CFG).

*/

class ControlNode : public SimpleDirectedGraphNode {

public:

  enum Type 
  {
    SGNODE,
    EMPTY
  };

  ControlNode(SgNode * node = NULL) : _node(node) {
    if (_node)
      _type = SGNODE;
    else
      _type = EMPTY;
  }

  SgNode * getNode() {return _node;}
  Type getType() {return _type;}

  int getID(ControlFlowGraph::ID_dir dir) {
    if (dir == ControlFlowGraph::FORWARD) {
      return _forID;
    } else {
      return _backID;
    }
  }

  void setID(int id, ControlFlowGraph::ID_dir dir) {
    if (dir == ControlFlowGraph::FORWARD) {
      _forID = id;
    } else {
      _backID = id;
    }
  }

  virtual void writeOut(std::ostream & os) {
    char buf[sizeof(ControlNode *)*2 + 3];
    sprintf(buf, "%p", this);
    os << "(" << _forID << "/" << _backID << ": " << buf << ") ";
    if (_type == EMPTY) {
      os << "EMPTY";
    } else {
      char buf[sizeof(SgNode *) * 2 + 3];
      sprintf(buf, "%p", _node);
      os << buf << ":" << _node->sage_class_name() << " ";
      os << "[" << escapeString(_node->unparseToString()) << "]";
    }
  }

private:

  SgNode * _node;
  Type _type;

  int _forID;
  int _backID;

};

// end of namespace: DominatorTreesAndDominanceFrontiers
 }
 
#endif
