#include "rose.h"

// Support for program Modeling
#include "programModel.h"

// Memory access counting
#include "memoryAccessModel.h"

int
main ( int argc, char* argv[] )
   {
     SgProject* project = frontend(argc,argv);
     ROSE_ASSERT (project != NULL);

#if 1
     printf ("Generate the dot output of the SAGE III AST \n");
     generateDOT ( *project );
#endif

  // Startup the Maple server
     MapleRoseInterface::startupMaple(argc,argv);

  // User-defined modeling of memory accesses
     AccessModeling::attachedAcessAttributes(project);

  // ROSE Support for general Modeling
     ProgramModeling::computeModelEquations(project);

  // Startup the Maple server
     MapleRoseInterface::stopMaple();

     return backend(project);
   }
