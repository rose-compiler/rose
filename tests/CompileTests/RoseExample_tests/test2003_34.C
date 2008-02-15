///////////////////////////////////////////////////////////////////////////////
// rose.C: Example (default) ROSE Preprocessor: used for testing ROSE

#include "rose.h"

using namespace std;

class MyTraversal
   : public SgSimpleProcessing
   {
     public:
          void visit(SgNode* astNode);
   };

void 
MyTraversal::visit ( SgNode* astNode )
   {
     SgGlobal *sg=isSgGlobal(astNode);
     string incstr="#include \"newadinc.h\"";
     if(sg!=NULL)
        {
       // DQ (8/15/2006): I had to comment this out to get it to compile with EDG, need to look into this!
       // MiddleLevelRewrite::insert(sg,incstr,MidLevelCollectionTypedefs::TopOfCurrentScope);
        }
   }

int
main( int argc, char * argv[] )
   {
     SgProject* project= frontend (argc,argv); 
     MyTraversal myTraversal;
     myTraversal.traverseInputFiles (project, postorder);
     project->unparse();
     return 0;
   }
/// end rose.C

#if 0
//////////////////////////////
///
//sample input

#include <math.h>
int i;

////////////////////////////////
#endif
