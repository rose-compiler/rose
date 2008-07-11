// Liao, 7/10/2008
// Demonstrate how to build sizeof() expressions
//-------------------------------------------------------------------
#include "rose.h"
#include <string>
using namespace SageBuilder;
using namespace SageInterface;

int main (int argc, char *argv[])
{
  // grab the scope in which AST will be added
  SgProject *project = frontend (argc, argv);
  SgGlobal *globalScope = getFirstGlobalScope (project);
  pushScopeStack(globalScope);

  // int j; 
  SgVariableDeclaration *varDecl_0 = buildVariableDeclaration("j", 
    buildIntType());
  appendStatement (varDecl_0);

  //int jc = sizeof(j);
  SgVariableDeclaration *varDecl_1 = buildVariableDeclaration("jc", 
    buildIntType(), buildAssignInitializer(buildSizeOfOp(buildVarRefExp("j"))));
  appendStatement (varDecl_1);

  // int p = sizeof(int);
  SgVariableDeclaration *varDecl_2 = buildVariableDeclaration("jp", 
    buildIntType(),buildAssignInitializer(buildSizeOfOp(buildIntType()))); 
  appendStatement (varDecl_2);

  popScopeStack ();

  AstTests::runAllTests(project);
  return backend (project);
}

