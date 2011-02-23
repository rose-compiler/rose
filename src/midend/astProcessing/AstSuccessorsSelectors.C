

// Author: Markus Schordan
// $Id: AstSuccessorsSelectors.C,v 1.4 2008/01/08 02:56:39 dquinlan Exp $

#include "sage3basic.h"
#include "roseInternal.h"
#include <typeinfo>
#include <algorithm>

#include "AstSuccessorsSelectors.h"
#include "Cxx_GrammarTreeTraversalAccessEnums.h"

// MS: 2003
void
AstSuccessorsSelectors::selectDefaultSuccessors(SgNode* node, SuccessorsContainer& succContainer) {
     ROSE_ASSERT (node != NULL);

     succContainer=node->get_traversalSuccessorContainer();
  // GB (09/26/2007): This code is not used anymore! There are a few special cases regarding traversals, but they are
  // now handled in ROSETTA (special code is generated there). The reason is that we want the new index-based traversals
  // to behave identically to the successor container based ones, so special cases have to be handled in a uniform way.
     // handle special cases
#if 0
#if ASTTRAVERSAL_USE_VISIT_FLAG==0
     if(SgClassDeclaration* n=dynamic_cast<SgClassDeclaration*>(node)) {
       // if the number of successors changes this code here probably needs to be adapted
       if (succContainer.size() != 1) {
         printf ("Error: succContainer.size() = %d node->sage_class_name() = %s \n",
                 succContainer.size(),node->sage_class_name());
       }
       ROSE_ASSERT(succContainer.size() == 1); 

       if(n->isForward()) {
         // nullify definiton to not be a traversal successor of a forward class/struct/union declaration
         succContainer[SgClassDeclaration_definition]=0;
       }
     }
#endif
#endif
}

// MS: 2003
void 
AstSuccessorsSelectors::selectReversePrefixSuccessors(SgNode* node, SuccessorsContainer& succContainer) {
  selectReverseBranchSuccessors(node,succContainer); // currently we do not filter declarations & scope statements
}

// MS: 2003
SgNode*
AstSuccessorsSelectors::leftSibling(SgNode* node) {
  ROSE_ASSERT(node!=0);
  SgNode* p=node->get_parent();
  if(p!=0) {
    SuccessorsContainer pSuccessors;
    selectDefaultSuccessors(p,pSuccessors);
    SuccessorsContainer::iterator nodeiter=std::find(pSuccessors.begin(),pSuccessors.end(),node);
    if(nodeiter != pSuccessors.begin() && nodeiter != pSuccessors.end()) // node is not first and node exists
      return *(--nodeiter); // return left sibling of 'node'
  } 
  return 0; // ('node' is the root node) or ('node' is first node) or ('node' not found) -> no left sibling
}

// MS: 2003
void 
AstSuccessorsSelectors::selectReverseBranchSuccessors(SgNode* node, SuccessorsContainer& succContainer) {
  // pre condition
  ROSE_ASSERT(node!=0);
  ROSE_ASSERT(succContainer.size()==0);

  SgNode* ls=leftSibling(node);
  if(ls!=0)
    succContainer.push_back(ls);
  else
    succContainer.push_back(node->get_parent()); // is null if root

  // post condition
  ROSE_ASSERT(succContainer.size()<=1);

  /*
  cout << "SUCCESSORS @" << node->sage_class_name() << ":";
  for(SuccessorsContainer::iterator i=succContainer.begin();i!=succContainer.end();i++) {
    cout << (*i)->sage_class_name() << " ";
  }
  cout << endl;
  */
}
