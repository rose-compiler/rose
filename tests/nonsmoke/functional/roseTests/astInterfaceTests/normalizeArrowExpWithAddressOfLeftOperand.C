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

  SageInterface::normalizeArrowExpWithAddressOfLeftOperand (project);

  // Generate source code from AST and call the vendor's compiler
  return backend(project);
}

