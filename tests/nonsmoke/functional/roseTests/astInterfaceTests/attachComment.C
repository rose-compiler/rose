#include "rose.h"
#include "sageBuilder.h"
#include <iostream>

using namespace std;
using namespace SageInterface;
using namespace SageBuilder;

int main (int argc, char *argv[])
{
  SgProject *project = frontend (argc, argv);

  // go to the function body
  SgFunctionDeclaration* mainFunc= findMain(project);

  attachComment(mainFunc, "Test for source comment."); 

  AstTests::runAllTests(project);
  return backend (project);
}


