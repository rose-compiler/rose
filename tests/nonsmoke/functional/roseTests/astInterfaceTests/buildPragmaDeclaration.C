// Liao, 1/30/2008
// Demostrate how to build an OpenMP pragma
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
  SgFunctionDeclaration* mainFunc= findMain(project);
  pushScopeStack (isSgScopeStatement (mainFunc->get_definition()->get_body()));

  // #pragma omp parallel
  SgPragmaDeclaration * decl = buildPragmaDeclaration("omp parallel");
  prependStatement (decl);
  popScopeStack ();

  AstTests::runAllTests(project);
  return backend (project);
}

