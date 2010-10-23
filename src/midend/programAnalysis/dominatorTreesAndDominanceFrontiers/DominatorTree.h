#ifndef _DOMINATORTREE_H_
#define _DOMINATORTREE_H_

#include "ControlFlowGraph.h"
#include <GraphDotOutput.h>
#include <map>

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

/*! \class DominatorTree
  
  This class constructs either a dominator or a post-dominator tree
  for a given function. 

  First, let us define "Dominance." A node X in a CFG dominates node Y
  if all paths into Y must first pass through X. (Similarly, a node X
  post-dominates node Y if all paths out of Y must pass through X)

  To understand a dominator tree, first the concept of immediate
  dominance must be understood: For a given node Y in a CFG, its
  "Immediate Dominator" X is the nearest node which dominates Y. (For
  example, within straight-line code, the Immediate Dominator of any
  statement is the statement immediately preceding; in an
  if-statement, the immediate dominator of the first statement in the
  true-body is the conditional). The dominator tree is thus a graph
  where the successors for a given node X are all the nodes for which
  X is the immediate dominator.

  A post-dominator tree is similar except calculated using
  post-dominators (hence, an immediate post-dominator for node X is
  the first statement Y that must be executed after X).

*/

class DominatorTree {

public:

  //! This enum holds values for determining whether the Tree is a dominator or post-dominator
  enum Direction 
  {
    PRE,  /*!< This indicates that we are building a dominator tree*/
    POST  /*!< This indicates that we are building a post-dominator tree*/
  };

  DominatorTree(SgNode * head, Direction d = PRE);

  //! get the CFG the dominator tree is built from
  ControlFlowGraph * getCFG() {return _cfg;}
  //! returns whether this is a dominator tree (PRE) or a post-dominator tree (POST)
  Direction getDirection() {return _dir;}
  //! returns the corresponding direction for the numbering of the CFG.
  ControlFlowGraph::ID_dir getCFGDirection() {return _iddir;}
  //! returns the number of nodes in the tree
  int getSize() {return _size;}

  //! for a given node, return the id of its immediate dominator
  int getDom(ControlNode * node) {return doms[node->getID(_iddir)];}
  //! for a given node id, return the id of its immediate dominator
  int getDom(int id) {return doms[id];}
					  
  void printCFG();
  void printDominators();


private:

  void _buildCFG(SgNode * head);
  void _setupStructures();

  void _findDominators();
  int _intersect(int f1, int f2);
  //! performs the next iteration of finding i-dom for a given node,
  //! returns whether the i-dom changed from the previous iteration.
  bool _processNode(int i);

  //! The control flow graph the dominator tree is built from
  ControlFlowGraph * _cfg;

  //! Indicates construction of a dominator or post-dominator tree
  Direction _dir;

  /*! Correlates with _dir, tells us whether the CFG is numbered
    forward (i.e. from the entry of the CFG, following forward edges),
    or backward (i.e. from the exit of the CFG, following backward
    edges).
  */
  ControlFlowGraph::ID_dir _iddir;

  int _size;

  //! Holds the immediate dominator for each ControlNode (indexed using CFG indices)
  int * doms;

};

// end of namespace: DominatorTreesAndDominanceFrontiers
 }
 
#endif
