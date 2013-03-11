#include "rose.h"

int main(int argc, char *argv[])
   {
  // Read source cdoe and build ROSE AST
     SgProject* project = frontend(argc,argv);

  // Run tests on AST
     AstTests::runAllTests(project);

     return backend(project);
   }
