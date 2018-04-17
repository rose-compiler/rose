// Liao, 1/15/2008
// Demostrate how to build a function using the SageBuilder interface 
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

  // defining  int foo(int x, float)
  // build parameter list first
  SgInitializedName* arg1 = buildInitializedName(SgName("x"),buildIntType());
  SgInitializedName* arg2 = buildInitializedName(SgName(""),buildFloatType());
  SgFunctionParameterList * paraList = buildFunctionParameterList();
  appendArg(paraList, arg1);  
  appendArg(paraList, arg2);  

  // DQ (8/28/2012): Modified the call to support the new API (need to think about if this is what we want as a new API).
  // build defining function declaration 
  // SageBuilder::buildDefiningFunctionDeclaration(const SgName& name, SgType* return_type, SgFunctionParameterList* paralist, SgScopeStatement* scope, SgExprListExp* decoratorList, bool buildTemplateInstantiation, SgFunctionDeclaration* first_nondefining_declaration, SgTemplateArgumentPtrList* templateArgumentsList)
  // SgFunctionDeclaration * func1 = buildDefiningFunctionDeclaration ("foo",buildVoidType(),paraList,NULL,NULL,false,NULL,NULL);
  // SgFunctionDeclaration * func1 = buildDefiningFunctionDeclaration (SgName("foo"),buildVoidType(),paraList);
  SgFunctionDeclaration * func1 = buildDefiningFunctionDeclaration ("foo",buildVoidType(),paraList,NULL);

	  // build a statement inside the function body
  SgBasicBlock *func_body = func1->get_definition ()->get_body ();
  ROSE_ASSERT (func_body);
  pushScopeStack (isSgScopeStatement (func_body));

  SgVariableDeclaration *varDecl = buildVariableDeclaration
    (SgName ("i"), buildIntType());
	  // Insert the statement
  appendStatement (varDecl);
  popScopeStack ();
	  // insert the defining function
  appendStatement (func1);

  // pop the final scope after all AST insertion
  popScopeStack ();

  AstTests::runAllTests(project);
  return backend (project);
}

