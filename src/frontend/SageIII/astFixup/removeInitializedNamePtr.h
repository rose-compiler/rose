
#ifndef __removeInit
#define __removeInit
#include "AstNodePtrs.h"

// [MS]: required because we use the STL find algorithm
// DQ (12/7/2003): use platform independent macro defined in config.h
// #include STL_ALGO_HEADER_FILE

// Permited to include AST testing
// #include "AstTraversal.h"

// [MS]
class RemoveInitializedNamePtrInheritedAttribute 
   {
     public:
          RemoveInitializedNamePtrInheritedAttribute()
             : firstInitializedNameVisited(false),
               parentNode(NULL) {}
          bool firstInitializedNameVisited;
          SgNode* parentNode;
   };

// [MS]
class RemoveInitializedNamePtr : public SgTopDownProcessing<RemoveInitializedNamePtrInheritedAttribute>
   {
     public:
       // DQ (1/18/2004): list of types that have been traversed
          static std::list<SgNode*> listOfTraversedNodes;

          RemoveInitializedNamePtrInheritedAttribute 
               evaluateInheritedAttribute ( SgNode* node, 
                                            RemoveInitializedNamePtrInheritedAttribute ia);
   };

// DQ (8/20/2005): Changed name of subTemporaryAstFixes() to removeInitializedNamePtr()
// to match the design of the other AST fixup operations in this directory.
void removeInitializedNamePtr(SgNode* node);

// [MS]: delete AST nodes which represent operators and do not have any successor.
// case1: SgDotExp: can have 2 successors, both can be null
class DeleteEmptyOperatorNodes : public AstNodePtrs 
   {
     protected:
       // DQ (1/18/2004): list of types that have been traversed
          static std::list<SgNode*> listOfTraversedTypes;

     protected:
          virtual void visitWithAstNodePointersList(SgNode* node, AstNodePointersList l);
   };

#endif
