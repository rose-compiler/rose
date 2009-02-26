/*
test code
by Liao, 2/26/2009
*/
#include "rose.h"
#include <iostream>
using namespace std;

int main(int argc, char * argv[])

{
  SgProject *project = frontend (argc, argv);
  SgFunctionDeclaration* func = SageInterface::findMain(project);
  ROSE_ASSERT(func != NULL);
  SgBasicBlock* body = func->get_definition()->get_body();
  ROSE_ASSERT(body!= NULL);
  SgStatement* stmt = SageInterface::getFirstStatement(body);
  ROSE_ASSERT(stmt != NULL);

  if (isSgForStatement(stmt))
  {
    bool result=false;
    result = SageInterface::forLoopNormalization(isSgForStatement(stmt));
    ROSE_ASSERT(result != false);
  }

  // generateWholeGraphOfAST("WholeAST");

  // run all tests
  AstTests::runAllTests(project);

  // Generate source code from AST and call the vendor's compiler
  return backend(project);
}

