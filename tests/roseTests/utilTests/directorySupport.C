// Example ROSE Translator
// used for testing ROSE infrastructure

#include "rose.h"

int
main( int argc, char * argv[] )
   {
     SgProject* project = frontend(argc,argv);
     AstTests::runAllTests(project);

  // Add a directory and unparse the code (to the new directory)
     SgDirectory* directory = new SgDirectory("xxx_new_directory_node_support");

     directory->set_parent(project);

#if 1
  // Add a directory to the list in the SgProject node.
     project->get_directoryList()->get_listOfDirectories().push_back(directory);

     SgFile* file = project->get_fileList()[0];

  // Put the file into the new directory.
     directory->get_fileList()->get_listOfFiles().push_back(file);

  // Erase the reference to the file in the project's file list.
     project->get_fileList().erase(find(project->get_fileList().begin(),project->get_fileList().end(),file));

     file->set_parent(directory);
#endif

#if 1
  // only the backend error code is reported
     return backend(project);
#else
     return 0;
#endif
   }
