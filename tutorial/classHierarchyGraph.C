// Example ROSE Preprocessor
// used for testing ROSE infrastructure

// #include <assert.h>
// #include <string>
#include "rose.h"
// #include <iomanip>
// #include "AstConsistencyTests.h"

#include <CallGraph.h>

using namespace std;

  int
main( int argc, char * argv[] )
   {
     SgProject* project = new SgProject(argc, argv); 

     ClassHierarchyWrapper hier( project );

     SgIncidenceDirectedGraph* classHier = hier.getClassHierarchyGraph();
     
     OutputDot::writeToDOTFile(classHier,"classHier.dot", "Class Hierarchy" );


     return 0;
   }

