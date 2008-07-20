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

  // Output DOT graph of current progress.
     generateDOT ( *project, "_before" );

#if 0
     const int MAX_NUMBER_OF_IR_NODES_TO_GRAPH_FOR_WHOLE_GRAPH = 2000;
  // printf ("Generate whole AST graph if small enough \n");
     generateAstGraph(project,MAX_NUMBER_OF_IR_NODES_TO_GRAPH_FOR_WHOLE_GRAPH);
#endif

  // Construct internal list of security flaw to consider for vulnerability detection and seeding
     SecurityFlaw::initialize();

  // Identify locations in the source code where security flaw vulnerabilities COULD exist.
     SecurityFlaw::detectAllVunerabilities(project);

  // Output DOT graph of current progress.
     generateDOT ( *project, "_afterIdentificationOfVulnerabilities" );

     SecurityFlaw::generationAllClones(project);

  // Output DOT graph of current progress.
     generateDOT ( *project, "_afterCloneGeneration" );

  // Modify the source code to introduce security flaws for the selected types of security flaws 
  // selected and at the location in the source code where the associated vulnerabilities could be seeded.
     SecurityFlaw::seedAllSecurityFlaws(project);

  // Output DOT graph of current progress.
     generateDOT ( *project, "_afterSeedingOfSecurityFlaws" );

#if 0
     const int MAX_NUMBER_OF_IR_NODES_TO_GRAPH_FOR_WHOLE_GRAPH = 2000;
  // printf ("Generate whole AST graph if small enough \n");
     generateAstGraph(project,MAX_NUMBER_OF_IR_NODES_TO_GRAPH_FOR_WHOLE_GRAPH);
#endif

  // Running internal tests (optional)
     AstTests::runAllTests (project);

  // Output the new code seeded with a specific form of bug.
     return backend (project);
   }
