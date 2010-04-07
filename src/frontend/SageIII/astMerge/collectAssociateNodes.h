#ifndef __collectAssNode
#define __collectAssNode
// Main function
void addAssociatedNodes ( SgNode* node, std::set<SgNode*> & nodeList, bool markMemberNodesDefinedToBeDeleted, SgNode* matchingNodeInMergedAST = NULL );

// Supporting functions
void addAssociatedNodes ( SgType*       type,       std::set<SgNode*> & nodeList, bool markMemberNodesDefinedToBeDeleted );
void addAssociatedNodes ( SgExpression* expression, std::set<SgNode*> & nodeList, bool markMemberNodesDefinedToBeDeleted );

#endif
