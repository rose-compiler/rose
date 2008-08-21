// This example demonstrates the seeding of a specific type
// of bug (buffer overflow) into any existing application to 
// test bug finding tools.

#include "rose.h"
using namespace SageBuilder;
using namespace SageInterface;

#include "bugSeeding.h"

void
banner( std::string s)
   {
  // This supports debugging...
     printf ("\n\n");
     printf ("**********************************************************\n");
     printf ("%s\n",s.c_str());
     printf ("**********************************************************\n");
     printf ("\n");
   }

void
annotateAST( SgProject* project )
   {
  // This function is atted to attach attributes to the AST that will make the generated 
  // DOT graphs more appropriate for presentation of concepts (like bug seeding).

     const SgFilePtrList & fileList = project->get_fileList();
     for (SgFilePtrList::const_iterator i = fileList.begin(); i != fileList.end(); i++)
        {
          PruningAttribute* pruneAttribute = new PruningAttribute();
          (*i)->addNewAttribute("PruningAttribute",pruneAttribute);
        }
   }

int
main (int argc, char *argv[])
   {
     SgProject *project = frontend (argc, argv);
     ROSE_ASSERT (project != NULL);

  // Running internal tests (optional)
     AstTests::runAllTests (project);

     banner("                 Annotate AST                    ");

#if 1
  // Add attributes to make the AST easier to view as a DOT graph.
     annotateAST(project);
#endif

  // Output DOT graph of current progress.
     generateDOT ( *project, "_before" );

#if 0
     const int MAX_NUMBER_OF_IR_NODES_TO_GRAPH_FOR_WHOLE_GRAPH = 2000;
  // printf ("Generate whole AST graph if small enough \n");
     generateAstGraph(project,MAX_NUMBER_OF_IR_NODES_TO_GRAPH_FOR_WHOLE_GRAPH);
#endif

  // Construct internal list of security flaw to consider for vulnerability detection and seeding
     SecurityFlaw::initialize();

     banner("           detectAllVunerabilities            ");

  // Identify locations in the source code where security flaw vulnerabilities COULD exist.
     SecurityFlaw::detectAllVunerabilities(project);

  // Output DOT graph of current progress.
     generateDOT ( *project, "_afterIdentificationOfVulnerabilities" );

     banner("             generationAllClones              ");

  // Do all required clone generation to support each "security vulnerability" using each 
  // "seeding methodology"; note that these multiply to generate a large number of clones 
  // on different parts of the AST.
     SecurityFlaw::generationAllClones(project);

  // Output DOT graph of current progress.
     generateDOT ( *project, "_afterCloneGeneration" );

     banner("             seedAllSecurityFlaws              ");

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
