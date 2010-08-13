// Liao, 1/15/2008
// Demnostrate how to build a function using the interface close to C language
//
// SageBuilder contains the AST nodes/subtrees builders
// SageInterface contains any other AST utitily tools 
//-------------------------------------------------------------------
#include "rose.h"

using namespace SageBuilder;
using namespace SageInterface;

int main (int argc, char *argv[])
{
  // grab the scope in which AST will be added
  SgProject *project = frontend (argc, argv);
  SgGlobal *globalScope = getFirstGlobalScope (project);
  pushScopeStack (isSgScopeStatement (globalScope));

  // build nondefining  int foo(int x, float)
  // build parameter list first
  SgInitializedName* arg1 = buildInitializedName(SgName("x"),buildIntType());
  SgInitializedName* arg2 = buildInitializedName(SgName(""),buildFloatType());
  SgFunctionParameterList * paraList = buildFunctionParameterList();
  appendArg(paraList,arg1);  
  appendArg(paraList,arg2);  
	  // build nondefining function declaration 
  SgFunctionDeclaration * func1 = buildNondefiningFunctionDeclaration \
     (SgName("foo"),buildVoidType(),paraList);
  appendStatement (func1);

    // SgFunctionParameterList should not be shared, deepcopy
  SgFunctionParameterList * paraList2 = isSgFunctionParameterList(deepCopy(paraList));
  SgFunctionDeclaration * func2 = buildNondefiningFunctionDeclaration \
     (SgName("foo"),buildVoidType(),paraList2);
  // insert prototype function declaration
  appendStatement (func2);

#if 1

  // build a defining function declaration

  SgFunctionParameterList * paraList3 = isSgFunctionParameterList(deepCopy(paraList));
  SgFunctionDeclaration *func3 =
    buildDefiningFunctionDeclaration (SgName("foo"),buildVoidType(),paraList3);

	  // build a statement inside the function body
  SgBasicBlock *func_body = func3->get_definition ()->get_body ();
  ROSE_ASSERT (func_body);
  pushScopeStack (isSgScopeStatement (func_body));

  SgVariableDeclaration *varDecl = buildVariableDeclaration
    (SgName ("i"), buildIntType());

	  // Insert the statement
  appendStatement (varDecl);
  popScopeStack ();
  // insert the defining function
  appendStatement (func3);

#endif 

#if 1
  // build nondefining  int foo(int x, float) the 3rd time
  SgFunctionParameterList * paraList4= isSgFunctionParameterList(deepCopy(paraList));
  SgFunctionDeclaration * func4 = buildNondefiningFunctionDeclaration \
     (SgName("foo"),buildVoidType(),paraList4);
  appendStatement (func4);
#endif 
 // pop the final scope after all AST insertion
  popScopeStack ();

  AstTests::runAllTests(project);
  return backend (project);
}

