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
  SgExprStatement* init_stmt= buildAssignStatement(buildVarRefExp("i"),buildIntVal(0));
  // for (..,;i++;..); not ++i;
  SgExprStatement* incr_stmt = NULL;
  incr_stmt=buildExprStatement(buildPlusPlusOp(buildVarRefExp("i"),SgUnaryOp::postfix));
  // for(..,..,i<100) It is an expression, not a statement!
  SgExpression* cond_exp=NULL;
  cond_exp = buildLessThanOp(buildVarRefExp("i"),buildIntVal(100)); 
   // j++; as loop body statement
   SgStatement* loop_body= NULL; 
  loop_body = buildExprStatement(buildPlusPlusOp(buildVarRefExp("j"),SgUnaryOp::postfix));


  SgForStatement*for_stmt = buildForStatement (init_stmt,incr_stmt,cond_exp,loop_body);
  appendStatement (for_stmt);

  popScopeStack ();

  AstTests::runAllTests(project);
  project->unparse();

  return 0;
}

