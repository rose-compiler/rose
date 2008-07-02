// This example demonstrates the seeding of a specific type
// of bug (buffer overflow) into any existing application to 
// test bug finding tools.

#include "rose.h"
using namespace SageBuilder;
using namespace SageInterface;

#include "bugSeeding.h"

int
main (int argc, char *argv[])
   {
     SgProject *project = frontend (argc, argv);
     ROSE_ASSERT (project != NULL);

  // Running internal tests (optional)
     AstTests::runAllTests (project);

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
  // Build a BufferOverFlowSecurityFlaw object
     BufferOverFlowSecurityFlaw bufferOverFlowSecurityFlaw;

  // Call the member function to annotate the AST where BufferOverFlowSecurityFlaw vulnerabilities exist.
     bufferOverFlowSecurityFlaw.detectVunerabilities(project);

  // Call the member function to seed security flaws into the AST (at locations where vulnerabilities were previously detected).
     bufferOverFlowSecurityFlaw.seedSecurityFlaws(project);
#else
     SecurityFlaw::buildAllVunerabilities();
     SecurityFlaw::detectAllVunerabilities(project);
     SecurityFlaw::seedAllSecurityFlaws(project);
  // SecurityFlaw::seedConedTrees(project);
#endif

  // Running internal tests (optional)
     AstTests::runAllTests (project);

  // Output the new code seeded with a specific form of bug.
     return backend (project);
   }
