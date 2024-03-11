#ifndef ROSE_sageMemory_H
#define ROSE_sageMemory_H

class SgNode;

// DQ (12/22/2005): Jochen's support for a constant (non-NULL) valued pointer
// to use to distinguish valid from invalid IR nodes within the memory pools.
/*! \brief This Namespace supports the file I/O for the AST.
 */
namespace AST_FileIO
   {
  /*! \brief This function returns a static pointer used with SgNode::p_freepointer.

      This function returns
      returns an internally held (function local data) static pointer value
      used by SgNode::p_freepointer to identify non-NULL value representing
      valid IR node in memory pool.  A NULL pointer represents last entry in
      the memory pool.  This value allows us to distinguish the entries, and not
      require additional memory storage on each IR node.
   */
     SgNode* IS_VALID_POINTER();

  /*! \brief Similar value as above function for reprentation of subsets of the AST
   */
     SgNode* TO_BE_COPIED_POINTER();
   }

#endif
