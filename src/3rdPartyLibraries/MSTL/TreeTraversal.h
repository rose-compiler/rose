// Author: Markus Schordan
// $Id: TreeTraversal.h,v 1.3 2006/04/24 00:21:27 dquinlan Exp $

// WORK IN PROGRESS. DO NOT USE.

#ifndef TREETRAVERSAL_H
#define TREETRAVERSAL_H

#include <vector>

template<typename TreeNode>
class TreeTraversal {
 public:  
  TreeTraversal();
  virtual ~TreeTraversal();
  virtual void traverse(TreeNode node)=0;
  virtual bool isNullNode(TreeNode node) { return node==0; /* as default we assume the comparison with 0 to succeed on a null-node */ }
  virtual void nullNodeVisit() { }
  virtual bool skipNode(TreeNode node) { return false; }
  virtual bool skipSubTreeOfNode(TreeNode node) { return false; }
 protected:
  typedef typename std::vector<TreeNode> ChildrenContainer; /* will be made a template parameter */
  virtual void setChildrenContainer(TreeNode node, ChildrenContainer& c)=0;
 private:
};

template<typename TreeNode>
class PreOrderTraversal : public TreeTraversal<TreeNode> {
public:
  void traverse(TreeNode root);
protected:
  virtual void preOrderVisit(TreeNode node)=0;
};

template<typename TreeNode>
class InOrderTraversal : public TreeTraversal<TreeNode> {
public:
  void traverse(TreeNode root);
protected:
  virtual void inOrderVisit(TreeNode node)=0;
};

template<typename TreeNode>
class PostOrderTraversal : public TreeTraversal<TreeNode> {
public:
  void traverse(TreeNode root);
protected:
  virtual void postOrderVisit(TreeNode node)=0;
};

template<typename TreeNode>
class PrePostOrderTraversal : public TreeTraversal<TreeNode> {
protected:
  virtual void preOrderVisit(TreeNode node)=0;
  virtual void postOrderVisit(TreeNode node)=0;
public:
  void traverse(TreeNode node);
};

// Author: Markus Schordan
// $Id: TreeTraversal.C,v 1.2 2006/04/24 00:21:27 dquinlan Exp $

template<class TreeNode>
TreeTraversal<TreeNode>::
TreeTraversal() 
{
}

template<class TreeNode>
TreeTraversal<TreeNode>::
~TreeTraversal()
{}

template<class TreeNode>
void 
PreOrderTraversal<TreeNode>::
traverse(TreeNode node) {
  typename PreOrderTraversal<TreeNode>::ChildrenContainer c;
  if(this->isNullNode(node)) {
    if(!this->skipNode(node)) {
      this->nullNodeVisit();
    }
    return;
  } else {
    if(!this->skipNode(node)) {
      preOrderVisit(node);
    }
  }
  if(!this->skipSubTreeOfNode(node)) {
    this->setChildrenContainer(node,c);
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
      this->nullNodeVisit();
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
      this->nullNodeVisit();
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
  if(this->isNullNode(node)) {
    if(!this->skipNode(node)) {
      this->nullNodeVisit();
    }
    return;
  } else {
    if(!this->skipNode(node)) {
      preOrderVisit(node);
    }
  }
  if(!this->skipSubTreeOfNode(node)) {
    this->setChildrenContainer(node,c);
    for(typename PrePostOrderTraversal<TreeNode>::ChildrenContainer::iterator iter=c.begin();iter!=c.end();iter++) {
      traverse(*iter);
    }
  }
  if(!this->skipNode(node)) {
    postOrderVisit(node);
  }
}

#endif
