// Example ROSE Translator
// used for testing ROSE infrastructure

#include "rose.h"

int
main( int argc, char * argv[] )
   {
     SgProject* project = frontend(argc,argv);
     AstTests::runAllTests(project);

#if 1
  // Add a directory and unparse the code (to the new directory)
     SgDirectory* directory = new SgDirectory("xxx_new_directory_node_support");

     directory->set_parent(project->get_directoryList());

  // Add a directory to the list in the SgProject node.
     project->get_directoryList()->get_listOfDirectories().push_back(directory);

     SgFile* file = project->get_fileList()[0];
  // SgFile* file = project->get_file(0);

  // Put the file into the new directory.
     directory->get_fileList()->get_listOfFiles().push_back(file);

  // Erase the reference to the file in the project's file list.
     project->get_fileList().erase(find(project->get_fileList().begin(),project->get_fileList().end(),file));

     file->set_parent(directory->get_fileList());
#else
  // SgFile* file = project->get_file(0);
     SgFile* file = project->get_fileList()[0];
     SageInterface::moveToSubdirectory ("xxx_new_directory_node_support",file);
#endif

#if 1
  // printf ("Generate the dot output of the SAGE III AST \n");
     generateDOT ( *project );
  // printf ("DONE: Generate the dot output of the SAGE III AST \n");
#endif

#if 1
  // DQ (6/14/2007): Added support for simpler function for generation of graph of whole AST.
     const int MAX_NUMBER_OF_IR_NODES_TO_GRAPH_FOR_WHOLE_GRAPH = 8000;
  // printf ("Generate whole AST graph if small enough \n");
     generateAstGraph(project,MAX_NUMBER_OF_IR_NODES_TO_GRAPH_FOR_WHOLE_GRAPH);
  // printf ("DONE: Generate whole AST graph if small enough \n");
#endif

#if 0
  // only the backend error code is reported
     return backend(project);
#else
     return 0;
#endif
   }
