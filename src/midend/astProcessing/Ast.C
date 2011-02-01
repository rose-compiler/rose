

// Author: Markus Schordan, Vienna University of Technology, 2004.
// $Id: Ast.C,v 1.3 2006/04/24 00:21:32 dquinlan Exp $

#include "sage3basic.h"
#include "Ast.h"

Ast::iterator&
Ast::iterator::operator++() {
  SgNode* p=node;
  if(numSuccessors(p)==0) {
    if(numRightSiblings(p)==0) {
      p=findNextNode(p);
    } else {
      p=nextRightSibling(p);
    }
  } else {
    p=first(successors(p));
  }
  node=p; 
  return *this;
}

SgNode*
Ast::iterator::findNextNode(SgNode* p) {
  while(p!=0 && numRightSiblings(p)==0) {
    SgNode* old_p;
    old_p=p;
    p=parent(p);
    ROSE_ASSERT(p!=old_p);
    // check if we have returned to start node
    if(p==startNode)
      p=0;
  }
  if(p!=0)
    p=nextRightSibling(p);
  return p;
}

Ast::Ast(SgNode* astNode):startNode(astNode) {
  /* only initializer list required */
}

Ast::NodeList 
Ast::successors(SgNode* astNode) {
  AstSuccessorsSelectors::SuccessorsContainer l1;
  AstSuccessorsSelectors::selectDefaultSuccessors(astNode,l1);
  // MS: 09/07/04: eliminate all null-pointers
  Ast::NodeList l2;
  unsigned int numDel=0;
  for(AstSuccessorsSelectors::SuccessorsContainer::iterator i=l1.begin();i!=l1.end();i++) {
    if( (*i) != 0 ) {
      l2.push_back(*i);
    } else {
      numDel++;
    }
  }
  ROSE_ASSERT(l2.size()+numDel==l1.size());
  return l2;
}

unsigned int 
Ast::numSuccessors(SgNode* astNode) {
  ROSE_ASSERT(astNode!=0);
  return successors(astNode).size();
}

Ast::NodeList 
Ast::rightSiblings(SgNode* astNode) {
  ROSE_ASSERT(astNode!=0);
  SgNode* p=parent(astNode);
  NodeList rightSiblings;
  if(p!=0) {
    NodeList l=successors(p);
    bool found=false;
    for(NodeList::iterator i=l.begin(); i!=l.end(); i++) {
      if(found)
        rightSiblings.push_back(*i);
      if(astNode==*i)
        found=true;
    }
  }
  return rightSiblings;
}

unsigned int 
Ast::numRightSiblings(SgNode* astNode) {
  return rightSiblings(astNode).size();
}

SgNode* 
Ast::nextRightSibling(SgNode* astNode) {
  ROSE_ASSERT(astNode!=0);
  NodeList rSib=rightSiblings(astNode);
  ROSE_ASSERT(rSib.size()>0);
  return *(rSib.begin());
}

SgNode* 
Ast::parent(SgNode* astNode) {
  return astNode->get_parent();
}
