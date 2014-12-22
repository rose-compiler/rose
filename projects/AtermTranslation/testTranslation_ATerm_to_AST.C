// DQ (9/27/2013): This is required to be defined for the 64bit ATerm support.
#if (__x86_64__ == 1)
// 64-bit machines are required to set this before including the ATerm header files.
   #define SIZEOF_LONG 8
   #define SIZEOF_VOID_P 8
#else
// 32-bit machines need not have the values defined (but it is required for this program).
   #define SIZEOF_LONG 4
   #define SIZEOF_VOID_P 4
#endif

#include "rose.h"

// #include "rose_aterm_api.h"
#include "aterm1.h"
#include "aterm2.h"

#include <libgen.h>
#include <boost/lexical_cast.hpp>
#include <string>
#include <iomanip>

#include <fstream>
#include <vector>
#include <set>
#include <assert.h>

#include "atermTranslation.h"

using namespace std;

int
main(int argc, char* argv[])
   {
  // This program test the conversion of the ATerm into the ROSE AST.

     string aterm_filename;
     printf ("argc = %d \n",argc);
     if (argc == 2)
        {
          aterm_filename = argv[1];
          printf ("aterm_filename = %s \n",aterm_filename.c_str());
        }
       else
        {
          printf ("Error: must specify name of aterm file on command line. \n");
          ROSE_ASSERT(false);
        }

     printf ("Opening the aterm file: %s \n",aterm_filename.c_str());

  // Read the text file format (could alternatively read the binary file format).
     FILE* aterm_file_TAF = fopen(aterm_filename.c_str(),"r");

     ROSE_ASSERT(aterm_file_TAF != NULL);
     printf ("DONE: Opening the aterm file: %s \n",aterm_filename.c_str());

     printf ("Calling ATinit \n");
     ATerm bottom;
     ATinit(argc, argv, &bottom);

     printf ("Reading the aterm from the file: %s \n",aterm_filename.c_str());
     ATerm term = ATreadFromTextFile(aterm_file_TAF);
     printf ("DONE: Reading the aterm from the file: %s \n",aterm_filename.c_str());

  // This builds required builtin types (primative types, int, float, double, etc.).
  // Either we initialize the type system or recognize unique type name string and 
  // build them as needed. I prefer a uniform approach for all types.
     printf ("Calling AtermSupport::initializeTypeSystem() \n");
     AtermSupport::initializeTypeSystem();

#if 1
  // DQ (9/17/2014): Adding test for conversion of Aterm back to AST.
     printf ("Testing the reverse process to generate the ROSE AST from the Aterm \n");
  // SgNode* rootOfAST = convertAtermToNode(term);
     SgNode* rootOfAST = AtermSupport::generate_AST(term);
     printf ("rootOfAST = %p = %s \n",rootOfAST,rootOfAST->class_name().c_str());

  // SgProject* project = isSgProject(rootOfAST);
     SgSourceFile* sourceFile = isSgSourceFile(rootOfAST);
     ROSE_ASSERT(sourceFile != NULL);
#endif

#if 0
  // We need a SgProject to support the graph generation (the ATem we generate is only for a SgSourceFile).
  // generateDOT(*project);
     generateDOTforMultipleFile(*project, "AFTER_ATERM");

  // Output an optional graph of the AST (the whole graph, of bounded complexity, when active)
     const int MAX_NUMBER_OF_IR_NODES_TO_GRAPH_FOR_WHOLE_GRAPH = 5000;
     generateAstGraph(project,MAX_NUMBER_OF_IR_NODES_TO_GRAPH_FOR_WHOLE_GRAPH,"AFTER_ATERM");
#endif

#if 1
     printf ("Program Terminated Normally \n");
#endif

     return 0;
   }

#if 0
int
main(int argc, char* argv[])
   {
  // This program test the conversion of the ROSE AST to n ATerm and back to a ROSE AST.

  // Generate a ROSE AST as input.
  // printf ("Building the ROSE AST \n");
     SgProject* project = frontend(argc,argv);
  // printf ("Done: Building the ROSE AST \n");

  // Output an optional graph of the AST (just the tree, when active)
  // generateDOT(*project);

     SgFile* roseFile = project->operator[](0);
     ROSE_ASSERT(roseFile != NULL);
     SgSourceFile* sourceFile = isSgSourceFile(roseFile);
     ROSE_ASSERT(sourceFile != NULL);

  // printf ("Calling ATinit \n");
     ATerm bottom;
     ATinit(argc, argv, &bottom);

  // printf ("Calling convertAstNodeToRoseAterm \n");
  // ATerm term = convertNodeToAterm(project);
  // ATerm term = convertNodeToAterm(sourceFile);
  // ATerm term = convertAstNodeToRoseAterm(sourceFile);

  // Actually this implementation in convertNodeToAterm already adds the pointer value to the aterm, so we can just return this Aterm directly.
     ATerm term = convertNodeToAterm(sourceFile);

  // printf ("DONE: Calling convertAstNodeToRoseAterm term = %p \n",term);

     ROSE_ASSERT (term != NULL);

     string roseAST_filename = project->get_file(0).getFileName();
     char* s = strdup(roseAST_filename.c_str());
     string file_basename = basename(s);

  // ATerm_Graph::graph_aterm_ast(term,file_basename);

#if 1
  // DQ (9/17/2014): Adding test for conversion of Aterm back to AST.
     printf ("Testing the reverse process to generate the ROSE AST from the Aterm \n");
     SgNode* rootOfAST = convertAtermToNode(term);
     printf ("rootOfAST = %p = %s \n",rootOfAST,rootOfAST->class_name().c_str());
#endif

  // generateDOT(*project);
     generateDOTforMultipleFile(*project, "AFTER_ATERM");

  // Output an optional graph of the AST (the whole graph, of bounded complexity, when active)
     const int MAX_NUMBER_OF_IR_NODES_TO_GRAPH_FOR_WHOLE_GRAPH = 5000;
     generateAstGraph(project,MAX_NUMBER_OF_IR_NODES_TO_GRAPH_FOR_WHOLE_GRAPH,"AFTER_ATERM");

#if 0
     printf ("Program Terminated Normally \n");
#endif

     return 0;
   }
#endif

