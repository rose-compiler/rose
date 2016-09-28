// Liao, 12/8/2010
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

  //---------------subroutine test1(arg1, arg2)---------
 	 // build parameter list first
  SgFunctionParameterList * paraList = buildFunctionParameterList();

	  // build a Fortran subroutine declaration 
  SgProcedureHeaderStatement* func1 = buildProcedureHeaderStatement\
     ("TEST1",buildVoidType(),paraList, 
     SgProcedureHeaderStatement::e_subroutine_subprogram_kind);

	  // build a statement inside the function body
  SgBasicBlock *func_body = func1->get_definition ()->get_body ();
  ROSE_ASSERT (func_body);
  pushScopeStack (isSgScopeStatement (func_body));

     // build a common statement
  SgExprListExp* exp_list = buildExprListExp(buildVarRefExp("x",func_body));
  SgCommonBlockObject* cb1= buildCommonBlockObject("omp_cb1",exp_list);
  SgCommonBlock* stmt = buildCommonBlock(cb1);
	  // Insert the statement
  appendStatement (stmt);

    // build a variable which is used by the common block
  SgVariableDeclaration* var_decl = buildVariableDeclaration("x", buildIntType(), NULL);  
  appendStatement (var_decl);

  popScopeStack ();
	  // insert the statement
  appendStatement (func1);
  popScopeStack ();

  AstTests::runAllTests(project);
  return backend (project);
}

