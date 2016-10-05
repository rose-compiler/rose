///////////////////////////////////////////////////////////////////////////////
// rose.C: Example (default) ROSE Preprocessor: used for testing ROSE
#include "rose.h"
#ifdef HAVE_CONFIG_H
// This avoids requiring the user to use config.h and follows 
// the automake manual request that we use <> instead of ""
#include <config.h>
#endif


#define CURRENT_FILE ""

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
           MiddleLevelRewrite::insert(sg,incstr,
MidLevelCollectionTypedefs::TopOfCurrentScope);
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
//////////////////////////////
///
//sample input

#include <math.h>
int i;

////////////////////////////////

