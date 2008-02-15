
#include "rose.h"

using namespace std;

void checkIsModifiedFlag( SgNode *node)
   {
  // DQ (7/7/2005): Introduce tracking of performance of ROSE.
     TimingPerformance timer ("Check the isModifiedFlag in each IR node:");

     CheckIsModifiedFlagSupport(node);
   }

// QY
void CheckIsModifiedFlagSupport( SgNode *node)
{
  if (node->get_isModified()) {
     //cerr << "Warning: the isModified flag is set in frontend for class " << node->sage_class_name() << endl;
     //assert(false);
     node->set_isModified(false);
  }
//  cerr << " visiting " << node->sage_class_name() << " : " << node << endl;
  vector <SgNode*> children = node->get_traversalSuccessorContainer(); 
  for (vector<SgNode*>::const_iterator p = children.begin(); p != children.end(); ++p) {
    SgNode *cur = *p;
     if ( cur != 0) {
         if (node == cur->get_parent())
             CheckIsModifiedFlagSupport(cur);
     }
  }
} 

