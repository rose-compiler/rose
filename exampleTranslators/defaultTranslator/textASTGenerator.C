
#include "rose.h"

int
main ( int argc, char** argv )
{
    ROSE_INITIALIZE;

     SgProject* project = frontend(argc,argv);

  // Run internal consistency tests on AST
     AstTests::runAllTests(project);
     std::string filename = SageInterface::generateProjectName(project)+".AST.txt";
     SageInterface::printAST2TextFile(project,filename.c_str());

     return backend(project);

}













