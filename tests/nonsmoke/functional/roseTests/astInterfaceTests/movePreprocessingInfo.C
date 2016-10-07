// Liao, 4/30/2008
// Demonstrate how to move preprocessing from one stmt to another
//
//-------------------------------------------------------------------
#include "rose.h"
#include <iostream>

using namespace std;
using namespace SageBuilder;
using namespace SageInterface;

int main (int argc, char *argv[])
{
  // grab the scope in which AST will be added
  SgProject *project = frontend (argc, argv);
  SgGlobal *globalScope = getFirstGlobalScope (project);
  pushScopeStack (isSgScopeStatement (globalScope));

     // build a parameter list (int x, string* y)
  SgInitializedName* arg1 = buildInitializedName(SgName("x"),buildIntType());
  SgInitializedName* arg2 = buildInitializedName(SgName("y"),buildPointerType(buildCharType()));
  SgFunctionParameterList * paraList = buildFunctionParameterList();
  appendArg(paraList,arg1);  
  appendArg(paraList,arg2);  
  // build a nondefining function declaration 
  SgFunctionDeclaration * func1 = buildNondefiningFunctionDeclaration \
     (SgName("foo"),buildVoidType(),paraList);

  SgStatement * oldFirstStmt = getFirstStatement(globalScope);
//  cout<<"debug..."<<oldFirstStmt->unparseToString()<<endl;
  prependStatement(func1);
  moveUpPreprocessingInfo(func1, oldFirstStmt);

  // build another nondefining function declaration 
  SgFunctionDeclaration * func2= buildNondefiningFunctionDeclaration(SgName("bar"),
          buildVoidType(),buildFunctionParameterList());
  
  oldFirstStmt = getFirstStatement(globalScope);
//  cout<<"debug..."<<oldFirstStmt->unparseToString()<<endl;
  prependStatement(func2);
  moveUpPreprocessingInfo(func2, oldFirstStmt);
// pop the final scope after all AST insertion
  popScopeStack ();
  AstTests::runAllTests(project);
  return backend (project);
}

