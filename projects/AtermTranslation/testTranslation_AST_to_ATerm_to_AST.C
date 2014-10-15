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
  // SgNode* rootOfAST = convertAtermToNode(term);
     SgNode* rootOfAST = AtermSupport::generate_AST(term);
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

