// SageBuilder contains all high level buildXXX() functions,
// such as buildVariableDeclaration(), buildLabelStatement() etc.
// SageInterface contains high level AST manipulation and utility functions,
// e.g. appendStatement(), lookupFunctionSymbolInParentScopes() etc.
#include "rose.h"
using namespace SageBuilder;
using namespace SageInterface;

int main (int argc, char *argv[])
{
  // Initialize and check compatibility. See rose::initialize
  ROSE_INITIALIZE;

  SgProject *project = frontend (argc, argv);

  // go to the function body of main()
  // and push it to the scope stack
  SgFunctionDeclaration* mainFunc= findMain(project);
  SgBasicBlock* body= mainFunc->get_definition()->get_body();
  pushScopeStack(body);

  // build a variable assignment statement: i=9;
  // buildVarRefExp(string varName) will automatically search for a matching variable symbol starting 
  // from the current scope to the global scope.
  SgExprStatement* assignStmt = buildAssignStatement(buildVarRefExp("i"),buildIntVal(9));

 // insert it before the last return statement
  SgStatement* lastStmt = getLastStatement(topScopeStack());
  insertStatementBefore(lastStmt,assignStmt); 

  popScopeStack();

  //AstTests ensures there is no dangling SgVarRefExp without a mathing symbol
  AstTests::runAllTests(project);
  return backend (project);
}

