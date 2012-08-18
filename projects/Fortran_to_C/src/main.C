/*
  Author: Pei-Hung Lin
  Contact: lin32@llnl.gov, phlin@cs.umn.edu

  Date Created       : July 5th, 2012
 
 This is a source to source compiler for Fortran

  Fortran to C translator 
  * input  : Fortran 77 source code
  * output : C source code

*/  

#include "rose.h"
#include "f2c.h"

using namespace std;
using namespace SageInterface;
using namespace Fortran_to_C;

bool isLinearlizeArray = true;
vector<SgArrayType*> arrayTypeList;

class typeTraversal : public ROSE_VisitorPattern
{
  public:
    void visit(SgArrayType* type);
};

void typeTraversal::visit(SgArrayType* type)
{
  arrayTypeList.push_back(type);
/*
*/
  
}

class f2cTraversal : public AstSimpleProcessing
{
  public:
    virtual void visit(SgNode* n);
};

void f2cTraversal::visit(SgNode* n)
{
  /*
    1. The following switch statement search for the Fortran-specific
       AST nodes and transform them into C nodes. 
    2. The new C nodes are created first.  Attributes and details
       are then copied from original Fortran nodes.  After the 
       copy, original Fortran nodes are deleted. 
  */
  switch(n->variantT())
  {
    case V_SgSourceFile:
      {
        SgFile* fileNode = isSgFile(n);
        translateFileName(fileNode);
      }
      break;
    case V_SgProgramHeaderStatement:
      {
        SgProgramHeaderStatement* ProgramHeaderStatement = isSgProgramHeaderStatement(n);
        ROSE_ASSERT(ProgramHeaderStatement);
        translateProgramHeaderStatement(ProgramHeaderStatement);
        // Deep delete the original Fortran SgProgramHeaderStatement
        deepDelete(ProgramHeaderStatement);
      }
      break;
    case V_SgProcedureHeaderStatement:
      {
        SgProcedureHeaderStatement* ProcedureHeaderStatement = isSgProcedureHeaderStatement(n);
        ROSE_ASSERT(ProcedureHeaderStatement);
        translateProcedureHeaderStatement(ProcedureHeaderStatement);
        // Deep delete the original Fortran ProcedureHeaderStatement.
        deepDelete(ProcedureHeaderStatement);
      }
      break;
    case V_SgFortranDo:
      {
        SgFortranDo* fortranDo = isSgFortranDo(n);
        ROSE_ASSERT(fortranDo);
        translateFortranDoLoop(fortranDo);
        // Deep delete the original fortranDo .
        deepDelete(fortranDo);
      }
      break;
    case V_SgPntrArrRefExp:
      {
        SgPntrArrRefExp* pntrArrRefExp = isSgPntrArrRefExp(n);
        ROSE_ASSERT(pntrArrRefExp);
        if(isLinearlizeArray)
        {
          linearizeArraySubscript(pntrArrRefExp);
        }
        else
        {
          translateArraySubscript(pntrArrRefExp);
        }
      }
      break;
    default:
      break;
  }
}

int main( int argc, char * argv[] )
{
// Build the AST used by ROSE
  SgProject* project = frontend(argc,argv);
  AstTests::runAllTests(project);   

  generateAstGraph(project,8000,"_orig");
  
  // Traversal with Memory Pool to search for arrayType
  typeTraversal translateArrayType;
  traverseMemoryPoolVisitorPattern(translateArrayType);

  for(vector<SgArrayType*>::iterator i=arrayTypeList.begin(); i<arrayTypeList.end(); ++i)
  {
    if(isLinearlizeArray)
    {
      linearizeArrayDeclaration(*i);
    }
    else
    {
      translateArrayDeclaration(*i);
    }
  }
  // Simple traversal, bottom-up, to translate the rest
  f2cTraversal f2c;
  f2c.traverseInputFiles(project,postorder);
      
/*
  1. There should be no Fortran-specific AST nodes in the whole
     AST graph after the translation. 
  
  TODO: make sure translator generating clean AST 
*/
    //generateDOT(*project);
    generateAstGraph(project,8000);
    return backend(project);
}

