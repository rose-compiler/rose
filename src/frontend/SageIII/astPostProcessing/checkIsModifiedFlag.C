
#include "rose.h"

using namespace std;

/** Clears the p_isModified bit for @p node and all children, recurisvely. Returns true iff node or one of its descendents had
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

    /* It seems that the genericFile stored in an SgAsmFile is not considered to be a child. [RPM 2008-12-02] */
    SgAsmFile *asmFile = isSgAsmFile(node);
    if (asmFile) {
        SgAsmGenericFile *ef = asmFile->get_genericFile();
        if (ef && CheckIsModifiedFlagSupport(ef))
            retval = true;
    }
    
    return retval;
} 

