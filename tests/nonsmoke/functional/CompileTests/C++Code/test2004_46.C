
/*
This is an example submitted by Beata.

// **********************************************
// sample input (anything with an if statement will do):

void foo()
{
 int i=1;
 int j;
 if(i>0)
 {
   j=i;
 }
 else
 {
   j=-i;
 }
 return;
}
*/


// This avoids requiring the user to use config.h and follows
// the automake manual request that we use <> instead of ""
#include "rose.h"
#include <config.h>
#endif


#include "rewrite.h"
#define CURRENT_FILE ""

class MyInsert
 : public SgSimpleProcessing
 {
   public:
     void visit(SgNode * astNode);
 };

void
MyInsert::visit ( SgNode* astNode )
{

 if(SgIfStmt * ifst=isSgIfStmt(astNode))
   {
   printf("found If stmt\n");
   Sg_File_Info *finfo=new Sg_File_Info(CURRENT_FILE, 1, 1);
   SgName temporary("temp");
   SgVariableDeclaration * stmt=new SgVariableDeclaration(finfo, 
temporary, new SgTypeInt());
   //SgReturnStmt * stmt=new SgReturnStmt(finfo);
   SgStatement * bb=ifst->get_scope();
   bb->insert_statement(ifst, stmt, TRUE);
  }
}

class MyTraversal
 : public SgSimpleProcessing
{
   public:
        void visit(SgNode* astNode);
};

void
MyTraversal::visit ( SgNode* astNode )
{
  SgStatement * stmt=isSgStatement(astNode);
  if(stmt)
  {
    string unparse=stmt->unparseToString();
    printf("found statement %s %s\n", stmt->sage_class_name(), 
unparse.c_str());
  }
}

int
main( int argc, char * argv[] )
{
 SgProject* project= frontend (argc,argv);
 MyInsert myInsert;
 myInsert.traverseInputFiles (project, postorder);
 MyTraversal myTraversal;
 myTraversal.traverseInputFiles (project, postorder);
 project->unparse();
 return 0;
}
/// end rose.C

