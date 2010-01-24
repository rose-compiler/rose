// Example ROSE Translator
// used for testing ROSE infrastructure

#include "rose.h"

int
main( int argc, char * argv[] )
   {
     SgProject* project = frontend(argc,argv);
     AstTests::runAllTests(project);

  // Add a directory and unparse the code (to the new directory)
     SgDirectory* directory = new SgDirectory("xxx");

     directory->set_parent(project);

#if 0
  // Add a directory to the list in the SgProject node.
     project->get_directoryList()->get_listOfDirectories().push_back(directory);

     SgFile* file = project->get_fileList()[0];
     directory->get_fileList()->get_listOfFiles().push_back(file);


     file->set_parent(directory);
#endif

     return backend(project); // only backend error code is reported
   }
