#ifndef DELETE_ORPHAN_NODES_H
#define DELETE_ORPHAN_NODES_H

class BuildOrphanListMemoryPoolTraversal 
   : public ROSE_VisitTraversal
   {
  // This traversal finds any IR nodes in the AST which are not pointed to by other IR nodes
  // these can be created in the copy mechanism where an IR nodes always builds a child node
  // by default and then the copy mechanism overwrites the pointer to the child node.

     public:
         std::set<SgNode*> parentList;
         std::set<SgNode*> childList;

      //! Required traversal function
          void visit (SgNode* node);

          virtual ~BuildOrphanListMemoryPoolTraversal() {};
   };

void deleteOrphanIRnodesInMemoryPool();

#endif // DELETE_ORPHAN_NODES_H

