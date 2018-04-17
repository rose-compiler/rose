// This example program demonstrates how static analysis can call dynamic analysis (unfinished).
// the point is to demonstrate how both dynamic and static analysis can be mixed within ROSE.

#include "rose.h"

using namespace std;

int
main(int argc, char** argv)
   {
     ios::sync_with_stdio();     // Syncs C++ and C I/O subsystems!

  // Generate the binary AST from which all static analysis starts
     SgProject* project = frontend(argc,argv);
     ROSE_ASSERT (project != NULL);

  // Internal AST consistancy tests.
     AstTests::runAllTests(project);

#if 0
     printf ("Generate the dot output of the SAGE III AST \n");
  // generateDOT ( *project );
     generateDOTforMultipleFile ( *project );
     printf ("DONE: Generate the dot output of the SAGE III AST \n");
#endif

#if 0
  // DQ (6/14/2007): Added support for simpler function for generation of graph of whole AST.
     const int MAX_NUMBER_OF_IR_NODES_TO_GRAPH_FOR_WHOLE_GRAPH = 2000;
     printf ("Generate whole AST graph if small enough \n");
     generateAstGraph(project,MAX_NUMBER_OF_IR_NODES_TO_GRAPH_FOR_WHOLE_GRAPH);
     printf ("DONE: Generate whole AST graph if small enough \n");
#endif


  // Now call the Intel Pin tool to do dynamic analysis...(no additional header files are required)


  // Gather dynamically derived data and do more static analysis...


  // Unparse the binary executable (as a binary, as an assembler text file, 
  // and as a dump of the binary executable file format details (sections)).
     return backend(project);
   }
