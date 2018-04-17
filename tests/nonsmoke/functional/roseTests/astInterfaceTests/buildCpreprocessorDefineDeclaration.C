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

  buildCpreprocessorDefineDeclaration(mainFunc, "#define UPCR_WANT_MAJOR 3"); 
  //buildCpreprocessorDefineDeclaration(mainFunc, "Test for source comment."); 

  AstTests::runAllTests(project);
  return backend (project);
}


