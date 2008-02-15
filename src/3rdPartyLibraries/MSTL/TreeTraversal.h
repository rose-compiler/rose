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
  TreeTraversal<TreeNode>::nullNodeVisit;
};

template<typename TreeNode>
class InOrderTraversal : public TreeTraversal<TreeNode> {
public:
  void traverse(TreeNode root);
protected:
  virtual void inOrderVisit(TreeNode node)=0;
  TreeTraversal<TreeNode>::nullNodeVisit;
};

template<typename TreeNode>
class PostOrderTraversal : public TreeTraversal<TreeNode> {
public:
  void traverse(TreeNode root);
protected:
  virtual void postOrderVisit(TreeNode node)=0;
  TreeTraversal<TreeNode>::nullNodeVisit;
};

template<typename TreeNode>
class PrePostOrderTraversal : public TreeTraversal<TreeNode> {
protected:
  virtual void preOrderVisit(TreeNode node)=0;
  virtual void postOrderVisit(TreeNode node)=0;
  TreeTraversal<TreeNode>::nullNodeVisit;
public:
  void traverse(TreeNode node);
};


// include the source file where explicit template instantiation is required 
// #ifdef HAVE_EXPLICIT_TEMPLATE_INSTANTIATION 
   #include "TreeTraversal.C" 
// #endif 

#endif
