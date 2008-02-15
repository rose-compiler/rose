/*! \brief  test SageInterface::findMain()

*/
#include "rose.h"
#include <iostream>
using namespace std;
using namespace SageInterface;

int main (int argc, char *argv[])
{
  SgProject *project = frontend (argc, argv);

  SgFunctionDeclaration* mainFunc= findMain(project);

  cout<<mainFunc->unparseToString()<<endl;
  // translation only
   project->unparse();

  //invoke backend compiler to generate object/binary files
  // return backend (project);
}

