
#include "rose.h"
#include "unparseMacro.h"

int main( int argc, char * argv[] ) 
   {
  // Build the AST used by ROSE

     std::vector<std::string> newArgv(argv,argv+argc);
     newArgv.push_back("-rose:wave");

     SgProject* project = frontend(newArgv);

     UnparseMacro::unparseMacroCalls(project);

  // Generate source code from AST and call the vendor's compiler
     return backend(project);
   }

