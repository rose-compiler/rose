// Liao, 1/15/2008
// Demostrate how to build a function using the interface close to C language
//
// HighLevel_Sage_Builder_Interface contains the AST nodes/subtrees builders
// SageInterface contains any other AST utitily tools 
// HighLevel_Sage_C_Builder_Interface
//-------------------------------------------------------------------
#include "rose.h"
#include "sageBuilder.h"

using namespace HighLevel_Sage_Builder_Interface;
using namespace SageInterface;

int main (int argc, char *argv[])
{
  // grab the scope in which AST will be added
  SgProject *project = frontend (argc, argv);
  SgGlobal *globalScope = getFirstGlobalScope (project);
  pushScopeStack (isSgScopeStatement (globalScope));

  // build nondefining  int foo(int x, float)
 	 // build parameter list first
  SgInitializedName* arg1 = buildInitializedName(SgName("x"),SgTypeInt::createType());
  SgInitializedName* arg2 = buildInitializedName(SgName(""),SgTypeFloat::createType());
  SgFunctionParameterList * paraList = buildFunctionParameterList();
  append_arg(paraList,arg1);  
  append_arg(paraList,arg2);  
	  // build nondefining function declaration 
  SgFunctionDeclaration * func1 = buildNonDefiningFunctionDeclaration \
     (SgName("foo"),SgTypeVoid::createType(),paraList);
  append_statement (func1);

    // SgFunctionParameterList should not be shared, deepcopy
  SgFunctionParameterList * paraList2 = isSgFunctionParameterList(deepCopy(paraList));
  SgFunctionDeclaration * func2 = buildNonDefiningFunctionDeclaration \
     (SgName("foo"),SgTypeVoid::createType(),paraList2);
  // insert prototype function declaration
  append_statement (func2);

#if 1

  // build a defining function declaration

  SgFunctionParameterList * paraList3 = isSgFunctionParameterList(deepCopy(paraList));
  SgFunctionDeclaration *func3 =
    buildDefiningFunctionDeclaration (SgName("foo"),SgTypeVoid::createType(),paraList3);

	  // build a statement inside the function body
  SgBasicBlock *func_body = func3->get_definition ()->get_body ();
  ROSE_ASSERT (func_body);
  pushScopeStack (isSgScopeStatement (func_body));

  SgVariableDeclaration *varDecl = buildVariableDeclaration
    (SgName ("i"), SgTypeInt::createType ());

	  // Insert the statement
  append_statement (varDecl);
  popScopeStack ();
  // insert the defining function
  append_statement (func3);

#endif 

#if 1
  // build nondefining  int foo(int x, float) the 3rd time
  SgFunctionParameterList * paraList4= isSgFunctionParameterList(deepCopy(paraList));
  SgFunctionDeclaration * func4 = buildNonDefiningFunctionDeclaration \
     (SgName("foo"),SgTypeVoid::createType(),paraList4);
  append_statement (func4);
#endif 
 // pop the final scope after all AST insertion
  popScopeStack ();

  AstTests::runAllTests(project);
  return backend (project);
}

