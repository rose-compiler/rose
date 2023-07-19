/*
test code

by Liao, 1/18/2023

*/
#include "rose.h"
#include <iostream>
using namespace std;

int main(int argc, char * argv[])

{
  SgProject *project = frontend (argc, argv);

  // transformationGeneratedOnly set to false for this test
  SageInterface::normalizeArrowExpWithAddressOfLeftOperand (project,false);

  // Generate source code from AST and call the vendor's compiler
  return backend(project);
}

