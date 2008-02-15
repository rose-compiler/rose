#include "sageBuilder.h"
#include <iostream>
using namespace std;
using namespace SageBuilder;
using namespace SageInterface;

int main (int argc, char *argv[])
{
  SgProject *project = frontend (argc, argv);

  insertHeader("stdio.h",true,getFirstGlobalScope(project));

  AstTests::runAllTests(project);
  return backend (project);
}


