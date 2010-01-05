#include <rose.h>

// DQ (2/2/2009): This will go into rose.h at some point.
#include <libraryIdentification.h>

using namespace std;
using namespace LibraryIdentification;

int
main(int argc, char** argv)
   {
     TimingPerformance timer ("AST Library Identification checker (main): time (sec) = ",true);

     SgProject* project = frontend(argc,argv);
     ROSE_ASSERT (project != NULL);

     ROSE_ASSERT(project->get_fileList() .empty() == false);

  // Just set the project, the report will be generated upon calling the destructor for "timer"
     timer.set_project(project);

  // Internal AST consistancy tests.
     AstTests::runAllTests(project);

  // Build a Library Identification database (in the current directory).
     generateLibraryIdentificationDataBase( "testLibraryIdentification.db", project );

#if 0
  // Match functions in AST against Library Identification database.
     matchAgainstLibraryIdentificationDataBase("testLibraryIdentification.db", project);
#else
     printf ("SKIPPING TEST OF BINARY AGAINST GENERATED DATABASE! \n");
#endif

#if 0
  // This is not well tested yet! Fails in: bool SgTreeTraversal<InheritedAttributeType, SynthesizedAttributeType>::inFileToTraverse(SgNode*)
     printf ("Generate the pdf output of the binary AST \n");
     generatePDF ( *project );
#endif

#if 1
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

#if 1
  // Unparse the binary executable (as a binary, as an assembler text file, 
  // and as a dump of the binary executable file format details (sections)).
     return backend(project);
#else
     ROSE_ASSERT(project->get_fileList() .empty() == false);
     printf ("project->get_file(0).get_isLibraryArchive() = %s \n",project->get_file(0).get_isLibraryArchive() ? "true" : "false");
     if (project->get_file(0).get_isLibraryArchive() == true)
        {
          printf ("WARNING: SKIPPING THE UNPARSING OF THE LIBRARY_ARCHIVE FILE \n");
          return 0;
        }
       else
        {
          return backend(project);
        }
#endif
   }
