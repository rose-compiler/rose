// Example ROSE Translator used for testing ROSE parser infrastructure. 
// This tests the ability of ROSE to process the source code, but then 
// uses the vendor compiler to generate the object file or executable.
#include "rose.h"

int
main( int argc, char * argv[] )
   {
     std::vector<std::string> sourceCommandline = std::vector<std::string>(argv, argv + argc);
     sourceCommandline.push_back("-rose:skip_parser");

     SgProject* project = frontend(sourceCommandline);
     return backendCompilesUsingOriginalInputFile(project);
   }
