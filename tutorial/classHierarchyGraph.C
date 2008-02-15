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

     ClassHierarchy* classHier = hier.getClassHierarchyGraph();
     GraphDotOutput outputHier (*classHier);
     outputHier.writeToDOTFile ("classHier.dot");

     return 0;
   }

