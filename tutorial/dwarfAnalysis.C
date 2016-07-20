#include "rose.h"

int
main(int argc, char** argv)
   {
  // Initialize and check compatibility. See rose::initialize
     ROSE_INITIALIZE;

     SgProject* project = frontend(argc,argv);
     ROSE_ASSERT (project != NULL);

#if 1
     int numberOfASTnodes = numberOfNodes();
     printf ("Generate the dot output of the SAGE III AST numberOfASTnodes = %d \n",numberOfASTnodes);
     if (numberOfASTnodes <= 20000)
        {
       // generateDOT ( *project );
          generateDOTforMultipleFile ( *project );
          printf ("DONE: Generate the dot output of the SAGE III AST \n");
        }
       else
        {
          printf ("Skipping generation of dot file (too large to be useful), run a smaller input program \n");
        }
#endif

  // Skip call to backend since this is just an analysis.

  // Unparse the binary executable (as a binary, as an assembly text file, 
  // and as a dump of the binary executable file format details (sections)).
  // return backend(project);
   }
