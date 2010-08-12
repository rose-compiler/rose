#ifndef COLLECT_ASSOCIATE_NODES_H
#define COLLECT_ASSOCIATE_NODES_H
// Main function
void addAssociatedNodes ( SgNode* node, std::set<SgNode*> & nodeList, bool markMemberNodesDefinedToBeDeleted, SgNode* matchingNodeInMergedAST = NULL );

// Supporting functions
void addAssociatedNodes ( SgType*       type,       std::set<SgNode*> & nodeList, bool markMemberNodesDefinedToBeDeleted );
void addAssociatedNodes ( SgExpression* expression, std::set<SgNode*> & nodeList, bool markMemberNodesDefinedToBeDeleted );

#endif
