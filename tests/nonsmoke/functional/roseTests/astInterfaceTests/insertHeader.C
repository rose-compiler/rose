#include "rose.h"
#include <iostream>
using namespace SageBuilder;
using namespace SageInterface;

int main (int argc, char *argv[])
{
  SgProject *project = frontend (argc, argv);

  insertHeader("stdio.h",PreprocessingInfo::after,true,getFirstGlobalScope(project));

  AstTests::runAllTests(project);
  return backend (project);
}


