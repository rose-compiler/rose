// Liao, 1/7/2011
//-------------------------------------------------------------------
#include "rose.h"
#include <string>
using namespace SageBuilder;
using namespace SageInterface;

int main (int argc, char *argv[])
{
  // grab the scope in which AST will be added
  SgProject *project = frontend (argc, argv);
 //------------------------------------------------
  // bottom up build, no previous knowledge of target scope
  SgLabelStatement * label_stmt_1 = buildLabelStatement("", NULL);

  SgFunctionDefinition * funcDef = findMain(project)->get_definition();
  ROSE_ASSERT(funcDef);
  SgBasicBlock* body = funcDef->get_body();
  prependStatement (label_stmt_1,body);
  int l_val = suggestNextNumericLabel(funcDef);
  assert (l_val == 10);
  setFortranNumericLabel(label_stmt_1, l_val);

 //------------------------------------------------
  // top down set implicit target scope info. in scope stack.
  pushScopeStack (body);
  SgLabelStatement * label_stmt_2 = buildLabelStatement("");
  //prependStatement (label_stmt_2);
  insertStatementAfter(label_stmt_1, label_stmt_2);
  l_val = suggestNextNumericLabel(funcDef);
  assert (l_val == 20);
  setFortranNumericLabel(label_stmt_2, l_val);
  popScopeStack ();

  //-------------- if () goto 
  SgExprStatement * cond_stmt = buildExprStatement(buildEqualityOp (buildIntVal(2), buildIntVal(1)));
  SgIfStmt * if_stmt = buildIfStmt(cond_stmt, buildBasicBlock(), buildBasicBlock());
  SgGotoStatement* gt_stmt = buildGotoStatement(label_stmt_1->get_numeric_label()->get_symbol());
#if 0  
  SgLabelRefExp* l_exp = buildLabelRefExp(label_stmt_1->get_numeric_label()->get_symbol());
  l_exp->set_parent(gt_stmt);
  gt_stmt->set_label_expression(l_exp);
#endif  
  appendStatement (gt_stmt, isSgScopeStatement(if_stmt->get_true_body()));

  appendStatement(if_stmt,body);
  AstTests::runAllTests(project);
  return backend (project);
}

