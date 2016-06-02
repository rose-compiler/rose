// Example translator demontrating Partial Redundancy Elimination (PRE).

#include "rose.h"
#include "CommandOptions.h"

int main (int argc, char* argv[])
   {
  // Initialize and check compatibility. See rose::initialize
     ROSE_INITIALIZE;

  // Build the project object (AST) which we will fill up with multiple files and use as a
  // handle for all processing of the AST(s) associated with one or more source files.
     std::vector<std::string> l = CommandlineProcessing::generateArgListFromArgcArgv(argc, argv);

     CmdOptions::GetInstance()->SetOptions(argc, argv);
     SgProject* project = frontend(l);

     PRE::partialRedundancyElimination(project);

     return backend(project);
   }
