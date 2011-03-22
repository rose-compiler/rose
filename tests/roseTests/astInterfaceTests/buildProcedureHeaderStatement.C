// Liao, 3/11/2009
// Demonstrate how to build Fortran subroutine/function using the SageBuilder interface 
//
// SageBuilder contains the AST nodes/subtrees builders
// SageInterface contains any other AST utility tools 
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
  SgInitializedName* arg1 = buildInitializedName(SgName("ARG1"),buildFloatType());
  SgInitializedName* arg2 = buildInitializedName(SgName("ARG2"),buildFloatType());
  SgFunctionParameterList * paraList = buildFunctionParameterList();
  appendArg(paraList, arg1);  
  appendArg(paraList, arg2);  

	  // build a Fortran subroutine declaration 
  SgProcedureHeaderStatement* func1 = buildProcedureHeaderStatement\
     ("TEST1",buildVoidType(),paraList, 
     SgProcedureHeaderStatement::e_subroutine_subprogram_kind);

	  // build a statement inside the function body
  SgBasicBlock *func_body = func1->get_definition ()->get_body ();
  ROSE_ASSERT (func_body);
  pushScopeStack (isSgScopeStatement (func_body));
          // ARG1 = ARG2 + 0.5
  SgExprStatement* stmt = buildExprStatement( buildAssignOp(buildVarRefExp("ARG1"), 
           buildAddOp(buildVarRefExp("ARG2"), buildFloatVal(0.5))) );
	  // Insert the statement
  appendStatement (stmt);
  popScopeStack ();
	  // insert the statement
  appendStatement (func1);
  popScopeStack ();

  //---------------------------REAL FUNCTION TEST1(ARG1, ARG2)------
         // build parameter list first
  arg1 = buildInitializedName(SgName("ARG1"),buildFloatType());
  arg2 = buildInitializedName(SgName("ARG2"),buildFloatType());
  paraList = buildFunctionParameterList();
  appendArg(paraList, arg1);
  appendArg(paraList, arg2);
     // build a Fortran subroutine declaration
  func1 = buildProcedureHeaderStatement\
     ("TEST2",buildFloatType(),paraList,
     SgProcedureHeaderStatement::e_function_subprogram_kind, globalScope);
          // insert the defining function
  appendStatement (func1,globalScope);

           // build a statement inside the function body
  func_body = func1->get_definition ()->get_body ();
  ROSE_ASSERT (func_body);
          // TEST1 =ARG1 + ARG2
  stmt = buildExprStatement( buildAssignOp(buildFunctionRefExp("TEST2",globalScope),
           buildAddOp(buildVarRefExp("ARG1",func_body), buildVarRefExp("ARG2",func_body))) );
          // Insert the statement
  appendStatement (stmt,func_body);

  // write to a dot file
  generateAstGraph(project,3000);

  AstTests::runAllTests(project);
  return backend (project);
}

