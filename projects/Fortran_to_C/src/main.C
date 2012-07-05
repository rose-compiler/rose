/*
  Author: Pei-Hung Lin
  Contact: phlin@cs.umn.edu

  Date Created       : July 5th, 2012
 
 This is a source to source compiler for Fortran

  Fortran to C translator 
  * input  : Fortran 77 source code
  * output : C source code

*/  

#include "rose.h"
#include "f2c/f2cStatement.h"

using namespace std;
using namespace Fortran_to_C;


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
        transFileName(file);
    }
    // Translate Fortran program statmeent
    SgProgramHeaderStatement* ProgramHeaderStatement = isSgProgramHeaderStatement(n);
    if (ProgramHeaderStatement != NULL)
    {
        transProgramHeaderStatement(ProgramHeaderStatement);
        ProgramHeaderStatement->set_parent(NULL);
    }
    SageInterface::deepDelete(ProgramHeaderStatement);

    // Translate Fortran subroutine statmeent
    SgProcedureHeaderStatement* ProcedureHeaderStatement = isSgProcedureHeaderStatement(n);
    if (ProcedureHeaderStatement != NULL)
    {
        transProcedureHeaderStatement(ProcedureHeaderStatement);
        ProcedureHeaderStatement->set_parent(NULL);
    }
    SageInterface::deepDelete(ProcedureHeaderStatement);
}



int main( int argc, char * argv[] )
{
// Build the AST used by ROSE
   SgProject* project = frontend(argc,argv);
   AstTests::runAllTests(project);   

// Build the traversal object
   f2cTraversal f2c;
   f2c.traverseInputFiles(project,postorder);


   generateDOT(*project);
   generateAstGraph(project,8000);
   return backend(project);
}

