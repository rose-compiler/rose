// Liao, 7/3/2008
// - topdown construction: go to the target scope, build vardecl with scope info.
//   - L1: int i;
// - bottomup construction: build vardecl directly, then insert it into scope
//   - L2: int j;
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
  SgLabelStatement * label_stmt_1 = buildLabelStatement("L1");

  SgFunctionDefinition * funcDef = findMain(project)->get_definition();
  ROSE_ASSERT(funcDef);
  SgBasicBlock* body = funcDef->get_body();
  prependStatement (label_stmt_1,body);
 //------------------------------------------------
  // top down set implicit target scope info. in scope stack.
  pushScopeStack (body);
  SgLabelStatement * label_stmt_2 = buildLabelStatement("L2");
  //prependStatement (label_stmt_2);
  insertStatementAfter(label_stmt_1, label_stmt_2);
  popScopeStack ();

  AstTests::runAllTests(project);
  return backend (project);
}

