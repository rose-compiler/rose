#include "rose.h"

// This code demonstrates the construction of binary AST
// output to stdout. It is useful to avoid recompilation 
// of source files since the AST construction can in some
// cases be significantly more time consuming than the 
// input of the binary formatted AST file (which is optimized 
// to be very fast).  Ths makes it simpler (and quicker) to
// support tools that would operate on large ASTs.

int main(int argc, char *argv[])
   {
  // Initialize and check compatibility. See rose::initialize
     ROSE_INITIALIZE;

  // Read source cdoe and build ROSE AST
     SgProject* sageProject = frontend(argc,argv);

  // Run tests on AST
     AstTests::runAllTests(sageProject);

  // Write out the AST as a binary formatted file (to stdout)
     AST_FILE_IO::startUp(sageProject) ;
     AST_FILE_IO::writeASTToFile("/dev/stdout");

     return 0;
   }
