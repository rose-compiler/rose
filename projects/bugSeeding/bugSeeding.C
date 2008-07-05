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
     generateDOT ( *project, "_before" );
#endif

#if 0
  // DQ (6/14/2007): Added support for simpler function for generation of graph of whole AST.
     const int MAX_NUMBER_OF_IR_NODES_TO_GRAPH_FOR_WHOLE_GRAPH = 8000;
  // printf ("Generate whole AST graph if small enough \n");
     generateAstGraph(project,MAX_NUMBER_OF_IR_NODES_TO_GRAPH_FOR_WHOLE_GRAPH);
  // printf ("DONE: Generate whole AST graph if small enough \n");
#endif

  // Construct internal list of security flaw to consider for vulnerability detection and seeding
     SecurityFlaw::buildAllVunerabilities();

  // Identify locations in the source code where security flaw vulnerabilities COULD exist.
     SecurityFlaw::detectAllVunerabilities(project);

#if 1
     generateDOT ( *project, "_afterIdentificationOfVulnerabilities" );
#endif

  // Modify the source code to introduce security flaws for the selected types of security flaws 
  // selected and at the location in the source code where the associated vulnerabilities could be seeded.
     SecurityFlaw::seedAllSecurityFlaws(project);

#if 1
     generateDOT ( *project, "_afterSeedingOfSecurityFlaws" );
#endif

#if 0
  // DQ (6/14/2007): Added support for simpler function for generation of graph of whole AST.
     const int MAX_NUMBER_OF_IR_NODES_TO_GRAPH_FOR_WHOLE_GRAPH = 8000;
  // printf ("Generate whole AST graph if small enough \n");
     generateAstGraph(project,MAX_NUMBER_OF_IR_NODES_TO_GRAPH_FOR_WHOLE_GRAPH);
  // printf ("DONE: Generate whole AST graph if small enough \n");
#endif

  // Running internal tests (optional)
     AstTests::runAllTests (project);

  // Output the new code seeded with a specific form of bug.
     return backend (project);
   }
