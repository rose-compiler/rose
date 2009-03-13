// Liao, 8/27/2008
// Demonstrate how to build a for statement 
/*

   void foo()
  {
    int i,j;
   for (i=0;i++;i<100)  
     j++;
  }

 */
#include "rose.h"

using namespace SageBuilder;
using namespace SageInterface;

int main (int argc, char *argv[])
{

  SgProject *project = frontend (argc, argv);
  SgGlobal* global = getFirstGlobalScope(project);
  pushScopeStack(global);

  SgFunctionParameterList * paraList = buildFunctionParameterList();

  // build defining function declaration, void foo() {}
  SgFunctionDeclaration * func_def = buildDefiningFunctionDeclaration \
    ("foo",buildVoidType(),paraList);
  appendStatement(func_def);
  popScopeStack ();

  // build a statement inside the function body
  SgBasicBlock *func_body = func_def->get_definition ()->get_body ();
  ROSE_ASSERT (func_body);
  pushScopeStack (func_body);
  // int i;
  SgVariableDeclaration* stmt1 = buildVariableDeclaration("i",buildIntType(),NULL); 
  appendStatement(stmt1);
  // int j;
  SgVariableDeclaration* stmt2 = buildVariableDeclaration("j",buildIntType(),NULL); 
  appendStatement(stmt2);
  //for(i=0;..)
  SgStatement* init_stmt= buildAssignStatement(buildVarRefExp("i"),buildIntVal(0));

  // for(..,i<100,...) It is an expression, not a statement!
  SgExprStatement* cond_stmt=NULL;
  cond_stmt= buildExprStatement(buildLessThanOp(buildVarRefExp("i"),buildIntVal(100))); 
 
  // for (..,;...;i++); not ++i;
  SgExpression* incr_exp = NULL;
  incr_exp=buildPlusPlusOp(buildVarRefExp("i"),SgUnaryOp::postfix);
  // j++; as loop body statement
   SgStatement* loop_body= NULL; 
  loop_body = buildExprStatement(buildPlusPlusOp(buildVarRefExp("j"),SgUnaryOp::postfix));


  SgForStatement*for_stmt = buildForStatement (init_stmt,cond_stmt,incr_exp,loop_body);
  appendStatement (for_stmt);
  popScopeStack ();

  //-------- build for(int k=0;k<100;k++) j++;
  //
  init_stmt = buildVariableDeclaration("k", buildIntType(), buildAssignInitializer(buildIntVal(0)), NULL);
  cond_stmt=  buildExprStatement(buildLessThanOp(buildVarRefExp("k",NULL),buildIntVal(100)));
  incr_exp=buildPlusPlusOp(buildVarRefExp("k",NULL),SgUnaryOp::postfix);
  loop_body=buildExprStatement(buildPlusPlusOp(buildVarRefExp("j"),SgUnaryOp::postfix));
  for_stmt = buildForStatement (init_stmt,cond_stmt,incr_exp,loop_body);
  appendStatement(for_stmt,func_body);
  // fix varRef to point to the right symbol since 
  // variable references to j is used before the loop is inserted to the scope containing j
  fixVariableReferences(func_body);
  
  AstTests::runAllTests(project);
  project->unparse();

  return 0;
}

