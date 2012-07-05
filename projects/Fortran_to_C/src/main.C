//Fortran to C translation

#include "rose.h"
#include "sageBuilder.h"
#include "f2c/f2cStatement.h"

using namespace std;
using namespace SageInterface;
using namespace SageBuilder;
using namespace AstFromString;


class f2cTraversal : public AstSimpleProcessing
{
  public:
    virtual void visit(SgNode* n);
};

void f2cTraversal::visit(SgNode* n)
{
     // Find SgFile node and rename the output file name.
     SgFile* file = isSgFile(n);
     if (file != NULL)
     {
       f2cFile(file);
     }
     // Translate Fortran program statmeent
     SgProgramHeaderStatement* ProgramHeaderStatement = isSgProgramHeaderStatement(n);
     if (ProgramHeaderStatement != NULL)
     {
       f2cProgramHeaderStatement(ProgramHeaderStatement);
       ProgramHeaderStatement->set_parent(NULL);
     }
     //deepDelete(ProgramHeaderStatement);

     // Translate Fortran subroutine statmeent
     SgProcedureHeaderStatement* ProcedureHeaderStatement = isSgProcedureHeaderStatement(n);
     if (ProcedureHeaderStatement != NULL)
     {
       f2cProcedureHeaderStatement(ProcedureHeaderStatement);
       ProcedureHeaderStatement->set_parent(NULL);
     }
     //deepDelete(ProcedureHeaderStatement);
}



int main( int argc, char * argv[] )
{
// Build the AST used by ROSE
   SgProject* project = frontend(argc,argv);
// Build the traversal object
   f2cTraversal f2c;
   f2c.traverseInputFiles(project,postorder);


   generateDOT(*project);
   //generateAstGraph(project,8000);
   return backend(project);
}

