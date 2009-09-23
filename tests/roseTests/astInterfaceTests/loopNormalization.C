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
  Rose_STL_Container<SgNode*> loops = NodeQuery::querySubTree(body,V_SgForStatement);

  for (size_t i=0; i< loops.size(); i++)
  {
    SgForStatement * cloop = isSgForStatement(loops[i]);
    ROSE_ASSERT(cloop != NULL);
    bool result=false;
    result = SageInterface::forLoopNormalization(cloop);
    ROSE_ASSERT(result != false);
  }
  // run all tests
  AstTests::runAllTests(project);

  // Generate source code from AST and call the vendor's compiler
  return backend(project);
}

