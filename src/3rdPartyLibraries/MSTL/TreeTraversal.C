// Author: Markus Schordan
// $Id: TreeTraversal.C,v 1.2 2006/04/24 00:21:27 dquinlan Exp $

#ifndef TREETRAVERSAL_C
#define TREETRAVERSAL_C

#include "TreeTraversal.h"

template<class TreeNode>
TreeTraversal<TreeNode>::
TreeTraversal() 
{
}

template<class TreeNode>
TreeTraversal<TreeNode>::
~TreeTraversal(void)
{}

template<class TreeNode>
void 
PreOrderTraversal<TreeNode>::
traverse(TreeNode node) {
  typename PreOrderTraversal<TreeNode>::ChildrenContainer c;
  if(isNullNode(node)) {
    if(!skipNode(node)) {
      nullNodeVisit();
    }
    return;
  } else {
    if(!skipNode(node)) {
      preOrderVisit(node);
    }
  }
  if(!skipSubTreeOfNode(node)) {
    setChildrenContainer(node,c);
    for(typename PreOrderTraversal<TreeNode>::ChildrenContainer::iterator iter=c.begin(); iter!=c.end(); iter++) {
      traverse(*iter);
    }
  }
}

template<class TreeNode>
void 
InOrderTraversal<TreeNode>::
traverse(TreeNode node) {
  typename InOrderTraversal<TreeNode>::childrenContainer c;
  if(isNullNode(node)) {
    if(!skipNode(node)) {
      nullNodeVisit();
    };
    return;
  }
  if(!skipSubTreeOfNode(node)) {
    setChildrenContainer(node,c);
    for(typename InOrderTraversal<TreeNode>::ChildrenContainer::iterator iter=c.begin();iter!=c.end(); ) {
      traverse(*iter);
      iter++;
      if(iter!=c.end) {
	if(!skipNode(node)) {
	  inOrderVisit(*iter); // only inorder-visit if the node is not first and not last of children
	}
      }
    }
  }
}

template<class TreeNode>
void 
PostOrderTraversal<TreeNode>::
traverse(TreeNode node) {
  typename PostOrderTraversal<TreeNode>::childrenContainer c;
  if(isNullNode(node)) {
    if(!skipNode(node)) {
      nullNodeVisit();
    }
    return;
  }
  if(!skipSubTreeOfNode(node)) {
    setChildrenContainer(node,c);
    for(typename PostOrderTraversal<TreeNode>::ChildrenContainer::iterator iter=c.begin();iter!=c.end();iter++) {
      traverse(*iter);
    }
  }
  if(!skipNode(node)) {
    postOrderVisit(node);
  }
}

template<class TreeNode>
void 
PrePostOrderTraversal<TreeNode>::
traverse(TreeNode node) {
  typename PrePostOrderTraversal<TreeNode>::ChildrenContainer c;
  if(isNullNode(node)) {
    if(!skipNode(node)) {
      nullNodeVisit();
    }
    return;
  } else {
    if(!skipNode(node)) {
      preOrderVisit(node);
    }
  }
  if(!skipSubTreeOfNode(node)) {
    setChildrenContainer(node,c);
    for(typename PrePostOrderTraversal<TreeNode>::ChildrenContainer::iterator iter=c.begin();iter!=c.end();iter++) {
      traverse(*iter);
    }
  }
  if(!skipNode(node)) {
    postOrderVisit(node);
  }
}

#endif

