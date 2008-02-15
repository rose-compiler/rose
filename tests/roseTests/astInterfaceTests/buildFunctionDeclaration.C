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

static  SgTreeCopy treeCopy;
int main (int argc, char *argv[])
{
  // grab the scope in which AST will be added
  SgProject *project = frontend (argc, argv);
  SgGlobal *globalScope = getFirstGlobalScope (project);
  pushScopeStack (isSgScopeStatement (globalScope));

  // defining  int foo(int x, float)
 	 // build parameter list first
  SgInitializedName* arg1 = buildInitializedName(SgName("x"),SgTypeInt::createType());
  SgInitializedName* arg2 = buildInitializedName(SgName(""),SgTypeFloat::createType());
  SgFunctionParameterList * paraList = buildFunctionParameterList();
  append_arg(paraList, arg1);  
  append_arg(paraList, arg2);  

	  // build defining function declaration 
  SgFunctionDeclaration * func1 = buildDefiningFunctionDeclaration \
     (SgName("foo"),SgTypeVoid::createType(),paraList);

	  // build a statement inside the function body
  SgBasicBlock *func_body = func1->get_definition ()->get_body ();
  ROSE_ASSERT (func_body);
  pushScopeStack (isSgScopeStatement (func_body));

  SgVariableDeclaration *varDecl = buildVariableDeclaration
    (SgName ("i"), SgTypeInt::createType ());
	  // Insert the statement
  append_statement (varDecl);
  popScopeStack ();
	  // insert the defining function
  append_statement (func1);

  // pop the final scope after all AST insertion
  popScopeStack ();

  AstTests::runAllTests(project);
  return backend (project);
}

