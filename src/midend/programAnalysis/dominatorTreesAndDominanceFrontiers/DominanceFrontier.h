#ifndef _DOMINANCEFRONTIER_H_
#define _DOMINANCEFRONTIER_H_

#include "DominatorTree.h"

// DQ (3/21/2006): Namespace introduced to hide redundent use of 
// ControlFlowGraph class also found in:
//      src/midend/programTransformation/partialRedundancyElimination
namespace DominatorTreesAndDominanceFrontiers {

/*! \class DominanceFrontier

  This class constructs the dominance (or post-dominance) frontiers
  for all nodes in a ControlFlowGraph. A dominance (post-dominance)
  frontier for node X is simply the set of nodes such that a given
  node Y from the set is not dominated (post-dominated) by X, but
  there is an immediate predecessor of Y that is dominated
  (post-dominated) by X.

  The type of frontier we construct is determined by the DominatorTree
  that DominanceFrontier is initialized with.

*/

class DominanceFrontier {

public:

  DominanceFrontier(DominatorTree * dt) : _dt(dt),
                                          _size(_dt->getSize()),
                                          _domFrontier(new std::set<int>[_size])
  {_buildFrontier();}

  /*! get the dominance frontier for a given node (these need to be
  referenced against the CFG to determine the actual nodes in the
  frontier
  */
  std::set<int> getFrontier(int id) {return _domFrontier[id];}

  void printFrontier();

private:

  void _buildFrontier();

  //! The dominator tree the dominance frontiers are based on
  DominatorTree * _dt;

  //! The number of nodes
  int _size;

  //! Holds the dominance frontier for each node in the CFG (indexed by node id)
  std::set<int> * _domFrontier;

};

// end of namespace: DominatorTreesAndDominanceFrontiers
 }
 
#endif
