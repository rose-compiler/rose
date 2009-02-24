// rose.C: Example (default) ROSE Preprocessor: used for testing ROSE infrastructure

/*
sample input:

void f(double x, double y) 
{
       double z;
        x=2*x;
        f(2,2);
        z= y/x;
}
*/
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
     SgBasicBlock* bb = isSgBasicBlock(astNode);
     SgGlobal *sg=isSgGlobal(astNode);
           
     string symbol_id="foo";
     string incstr="#include \"newadinc.h\"";
           
     if (bb!=NULL)
        {
       // insert declaration double foo;
       // on top of this block
          Sg_File_Info * finfo=new Sg_File_Info(bb->getFileName(), 1,1);
          SgName sgnm(symbol_id.c_str());
          SgType * ptype = new SgTypeDouble(); 
          SgVariableDeclaration *sgdecl=new SgVariableDeclaration(finfo,sgnm,
ptype);
          bb->prepend_statement(sgdecl);
       // MiddleLevelRewrite::insert(bb, sgdecl->unparseToString(),MidLevelCollectionTypedefs::TopOfCurrentScope);
        } 
       else 
        {
          if(sg!=NULL)
             {
               MiddleLevelRewrite::insert(sg,incstr, MidLevelCollectionTypedefs::TopOfCurrentScope);
             }
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

