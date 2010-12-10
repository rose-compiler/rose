// Liao, 12/6/2010
// Demonstrate how to build Fortran External statement
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

  //---------------subroutine test1()---------
 	 // build parameter list first
  SgFunctionParameterList * paraList = buildFunctionParameterList();

	  // build a Fortran subroutine declaration 
  SgProcedureHeaderStatement* func1 = buildProcedureHeaderStatement\
     ("TEST1",buildVoidType(),paraList, 
     SgProcedureHeaderStatement::e_subroutine_subprogram_kind);

	  // build a statement inside the function body
  SgBasicBlock *func_body = func1->get_definition ()->get_body ();
  ROSE_ASSERT (func_body);
	  // insert the statement
  appendStatement (func1);
   // Add an external statement here
  SgAttributeSpecificationStatement* external_stmt1 = buildAttributeSpecificationStatement(SgAttributeSpecificationStatement::e_externalStatement); 
  SgFunctionRefExp *func_ref1 = buildFunctionRefExp ("TEST2", buildFunctionType(buildVoidType(), buildFunctionParameterTypeList()), func_body); 
  external_stmt1->get_parameter_list()->prepend_expression(func_ref1);
  func_ref1->set_parent(external_stmt1->get_parameter_list());
  prependStatement(external_stmt1, func_body);
 
 //---------------subroutine test2()---------
 	 // build parameter list first
  SgFunctionParameterList * paraList2 = buildFunctionParameterList();

	  // build a Fortran subroutine declaration 
  SgProcedureHeaderStatement* func2 = buildProcedureHeaderStatement\
     ("TEST2",buildVoidType(),paraList2, 
     SgProcedureHeaderStatement::e_subroutine_subprogram_kind);

	  // build a statement inside the function body
  SgBasicBlock *func_body2 = func2->get_definition ()->get_body ();
  ROSE_ASSERT (func_body2);
	  // insert the statement
  appendStatement (func2);
   // Add an external statement here
   // this external function is not declared in the current file at all
  SgAttributeSpecificationStatement* external_stmt = buildAttributeSpecificationStatement(SgAttributeSpecificationStatement::e_externalStatement); 
  SgFunctionRefExp *func_ref = buildFunctionRefExp ("parallel_start", buildFunctionType(buildVoidType(), buildFunctionParameterTypeList()), func_body2); 
  external_stmt->get_parameter_list()->prepend_expression(func_ref);
  func_ref->set_parent(external_stmt->get_parameter_list());
  prependStatement(external_stmt, func_body2);
 
  // ---            add an external statement ----------
  // with a reference to a 
   
  popScopeStack ();
  // write to a dot file
  generateAstGraph(project,3000);

  AstTests::runAllTests(project);
  return backend (project);
}

