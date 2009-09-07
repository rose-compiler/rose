// Example ROSE Preprocessor
// used for testing ROSE infrastructure
#include "rose.h"
#include <CallGraph.h>

using namespace std;

int
main( int argc, char * argv[] )
   {
     SgProject* project = new SgProject(argc, argv); 

     //Construct class hierarchy graph
     ClassHierarchyWrapper hier( project );

     //Output class hierarchy graph to dot
     AstDOTGeneration dotgen;
     dotgen.writeIncidenceGraphToDOTFile( hier.getClassHierarchyGraph() , "classHier.dot");

     return 0;
   }

