/*! \brief  test SageBuilder::buildFunctionCallStmt()
*   It can
*   - build a function call statement when the function is already declared.
*   - build a function call statement without previous declaration, a header is inserted first
*/
#include "rose.h"
#include <iostream>
using namespace std;
using namespace SageInterface;
using namespace SageBuilder;

int main (int argc, char *argv[])
{
  SgProject *project = frontend (argc, argv);

  //MiddleLevelRewrite::insert() is not stable
  // we only deal with one file here, so only one global scope
  SgGlobal* globalscope = getFirstGlobalScope(project);
  // insert header
#if 0
 // using MiddleLevelRewrite to parse the content of the header, but NOT working!
  MiddleLevelRewrite::ScopeIdentifierEnum scope = MidLevelCollectionTypedefs::StatementScope;
  MiddleLevelRewrite::PlacementPositionEnum locationInScope = \
	MidLevelCollectionTypedefs::TopOfCurrentScope;
  MiddleLevelRewrite::insert(globalscope,"#include \"inputbuildFunctionCalls.h\" \n",scope,locationInScope);
#else
  
  insertHeader("inputbuildFunctionCalls.h",PreprocessingInfo::after,false,globalscope);
#endif
  // go to the function body
  SgFunctionDeclaration* mainFunc= findMain(project);
  SgBasicBlock* body= mainFunc->get_definition()->get_body();
  pushScopeStack(body);

  // void foo(int p_sum)
  SgType* return_type = buildVoidType();
  SgVarRefExp* arg1 = buildVarRefExp(SgName("p_sum"));//type is inferred from symbol table
  SgExprListExp* arg_list = buildExprListExp();
  appendExpression(arg_list,arg1);
  SgExprStatement* callStmt1 = buildFunctionCallStmt(SgName("foo"),return_type, arg_list);

      // insert before the last return statement
  SgStatement* lastStmt = getLastStatement(topScopeStack());
  insertStatement(lastStmt,callStmt1); 
#if 1
  //int bar(double); it is declared in a header
  //build call stmt then
  SgType* return_type_2 = buildIntType();
  SgDoubleVal* arg_2 = buildDoubleVal(0.5); 
  SgExprListExp* arg_list_2 = buildExprListExp();
  appendExpression(arg_list_2,arg_2);
  SgExprStatement* callStmt_2 = buildFunctionCallStmt(SgName("bar"),return_type_2, arg_list_2);
 
      // insert before the last return statement
  lastStmt = getLastStatement(topScopeStack());
  insertStatement(lastStmt,callStmt_2); 
#endif
  popScopeStack();
  AstPostProcessing(project);

  AstTests::runAllTests(project);

  return backend (project);
}

