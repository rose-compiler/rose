// tps (01/14/2010) : Switching from rose.h to sage3.
#include "sage3basic.h"
#include "checkIsModifiedFlag.h"
using namespace std;

/** Clears the p_isModified bit for @p node and all children, recursively. Returns true iff node or one of its descendants had
 *  p_isModified set. */
bool checkIsModifiedFlag( SgNode *node)
   {
  // DQ (7/7/2005): Introduce tracking of performance of ROSE.
     TimingPerformance timer ("Check the isModifiedFlag in each IR node:");

     return CheckIsModifiedFlagSupport(node);
   }

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

