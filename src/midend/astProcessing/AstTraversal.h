// Author: Markus Schordan
// $Id: AstTraversal.h,v 1.3 2006/04/24 00:21:32 dquinlan Exp $

#ifndef ASTTRAVERSAL_H
#define ASTTRAVERSAL_H


#include "TreeTraversal.h"
//#include "sage3.h"

// WORK IN PROGRESS. DO NOT MODIFY OR THINK ABOUT IT.

// test new traversal classes
// example class for tree traversal
class AstPreOrderTraversal : public PreOrderTraversal<SgNode*> {
protected:
  virtual void preOrderVisit(SgNode* node);
  virtual void setChildrenContainer(SgNode* node, std::vector<SgNode*>& c);
};

class AstPrePostOrderTraversal : public PrePostOrderTraversal<SgNode*> {
protected:
  virtual void setChildrenContainer(SgNode* node, std::vector<SgNode*>& c);
};

//! AstCycleTest is based on the successor information of SgNodes (the
//! same information that is used by the traversals). It tests such
//! that it allows a preorder traversal to revisit nodes but reports an
//! error if the traversal would run into a cycle. If a cycle is found
//! it reports the list of SgNodes that are part of the cycle to
//! stdout, starting with "CYCLE FOUND: ..." and stops testing.
//! Usage:
//! AstCycleTest t;
//! t.traverse(SgNode* n); // where n is the root node of the subtree to be tested.
class AstCycleTest : public AstPrePostOrderTraversal {
public:
  //! determines whether the given sequence l of nodes extended by node creates a cycle
  //! the found cycle is returned. If no cycle is found, the returned list is empty.
  std::list<SgNode*> determineCycle(std::list<SgNode*>& l, SgNode* node);
  std::list<SgNode*> activeNodes;
  virtual void preOrderVisit(SgNode* node);

  //! In case of a cycle the traversal does not continue to prevent an infinite recursion
  //! of the traversal.
  virtual void setChildrenContainer(SgNode* node, std::vector<SgNode*>& c);
  virtual void postOrderVisit(SgNode* node);
  virtual void modifyChildrenContainer(SgNode*, std::vector<SgNode*>&) {}
};

#endif

