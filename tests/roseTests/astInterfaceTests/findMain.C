/*! \brief  test SageInterface::findMain() and SageBuilder::buildAssignStatement()

*/
#include "rose.h"
#include <iostream>
using namespace SageInterface;
using namespace SageBuilder;

int main (int argc, char *argv[])
{
  SgProject *project = frontend (argc, argv);

  // go to the function body
  SgFunctionDeclaration* mainFunc= findMain(project);
  std::cout<<mainFunc->unparseToString()<<std::endl;

  // translation only
   project->unparse();

  //invoke backend compiler to generate object/binary files
  // return backend (project);
}

