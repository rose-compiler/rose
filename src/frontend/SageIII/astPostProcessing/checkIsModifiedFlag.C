// tps (01/14/2010) : Switching from rose.h to sage3.
#include "sage3basic.h"
#include "checkIsModifiedFlag.h"
using namespace std;

#if 0
// DQ (4/16/2015): Replaced with better implementation.

/** Clears the p_isModified bit for @p node and all children, recursively. Returns true iff node or one of its descendants had
 *  p_isModified set. */
bool checkIsModifiedFlag( SgNode *node)
   {
  // DQ (7/7/2005): Introduce tracking of performance of ROSE.
     TimingPerformance timer ("Check the isModifiedFlag in each IR node:");

     return CheckIsModifiedFlagSupport(node);
   }
#endif

#if 0
// DQ (4/16/2015): Replaced with better implementation.

// QY
bool
CheckIsModifiedFlagSupport(SgNode *node)
{
    bool retval=false;

 // DQ (7/21/2010): This appears to be consuming 8-9 Gig of memory for the case of ROSE compiling a large source file.
 // RPM (8/2/2010): However, binary unparsing depends on being able to see when the AST has been modified.
#if 1
    if (node->get_isModified()) {
        node->set_isModified(false);
        retval = true;
    }

    vector <SgNode*> children = node->get_traversalSuccessorContainer(); 
    for (vector<SgNode*>::const_iterator p = children.begin(); p != children.end(); ++p) {
        SgNode *cur = *p;
        if (cur && node==cur->get_parent() && CheckIsModifiedFlagSupport(cur))
            retval = true;
    }
#endif

    return retval;
} 
#endif

ROSE_DLL_API void
reportNodesMarkedAsModified(SgNode *node)
   {
  // DQ (4/15/2015): This function reports using an output message and nodes marked as isModified (useful for debugging).

     class NodesMarkedAsModified : public AstSimpleProcessing
        {
          public:
               void visit (SgNode* node)
                  {
                    if (node->get_isModified() == true)
                       {
                         printf ("reportNodesMarkedAsModified(): node = %p = %s \n",node,node->class_name().c_str());
                       }
                  }
        };

  // Now buid the traveral object and call the traversal (preorder) on the AST subtree.
     NodesMarkedAsModified traversal;
     traversal.traverse(node, preorder);
   }

ROSE_DLL_API void
unsetNodesMarkedAsModified(SgNode *node)
   {
  // DQ (4/16/2015): This function sets the isModified flag on each node of the AST to false.

#if 0
  // DQ (6/4/2019): Debuggin the outlining to a seperate file (friend function inserted as 
  // part of transformation is marked as modified (but is reset, unclear how this is done).
     printf ("In unsetNodesMarkedAsModified(): node = %p = %s \n",node,node->class_name().c_str());
#endif

     class NodesMarkedAsModified : public AstSimpleProcessing
        {
          public:
               void visit (SgNode* node)
                  {
                    if (node->get_isModified() == true)
                       {
#if 0
                         printf ("unsetNodesMarkedAsModified(): node = %p = %s \n",node,node->class_name().c_str());
#endif
                      // Note that the set_isModified() functions is the only set_* access function that will not set the isModified flag.
                         node->set_isModified(false);
                       }
                  }
        };

  // Now buid the traveral object and call the traversal (preorder) on the AST subtree.
     NodesMarkedAsModified traversal;
     traversal.traverse(node, preorder);
   }

bool
checkIsModifiedFlag(SgNode *node)
   {
  // DQ (4/16/2015): This function is a reimplementation of the previous version which used to much space on large programs.

#if 0
  // DQ (6/4/2019): Debugging the outlining to a seperate file (friend function inserted as 
  // part of transformation is marked as modified (but is reset, unclear how this is done).
     printf ("In checkIsModifiedFlag(): node = %p = %s \n",node,node->class_name().c_str());
#endif

  // This function reproduces the functionality of the original checkIsModifiedFlag() function by both
  // returning a bool value if any subtree was marked as isModified and also resetting the isModified flags.

     class NodesMarkedAsModified : public AstSimpleProcessing
        {
          public:
               bool isSubtreeModifiedFlag;

               NodesMarkedAsModified () : isSubtreeModifiedFlag(false) {}

               void visit (SgNode* node)
                  {
                    if (node->get_isModified() == true)
                       {
                         isSubtreeModifiedFlag = true;

                      // Note that the set_isModified() functions is the only set_* access function that will not set the isModified flag.
                         node->set_isModified(false);
                       }
                  }
        };

  // Now buid the traveral object and call the traversal (preorder) on the AST subtree.
     NodesMarkedAsModified traversal;
     traversal.traverse(node, preorder);

     return traversal.isSubtreeModifiedFlag;
   }
