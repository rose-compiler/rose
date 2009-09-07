// Example ROSE Preprocessor
// used for testing ROSE infrastructure

// #include <assert.h>
// #include <string>
#include "rose.h"
// #include <iomanip>
// #include "AstConsistencyTests.h"

#include <CallGraph.h>

using namespace std;

/* Instructions for creating the filter database files 
   (to limit the volume of functions in the call graph)

About creating the filtering table:

0. run testCallGraphAnalysis on a file with some includes (like stdlib, iostream, etc.)
1. start sqlite: {path}/sqlite3 DATABASE
// assuming the name of the database file is DATABASE now type:
2. ATTACH DATABASE "__filter.db" AS f;
3. create table f.files as select * from graph g where g.filename > /";
4. .quit
5. rerun your program (this time, the functions in files in absolute paths should be filtered out
*/

int
main( int argc, char * argv[] )
   {
     SgProject* project = new SgProject(argc, argv); //test.getSgProject();

     CallGraphBuilder CGBuilder( project );
     CGBuilder.buildCallGraph();

     AstDOTGeneration dotgen;
     dotgen.writeIncidenceGraphToDOTFile(CGBuilder.getGraph(), "callGraph.dot");

     cout << "Generating DOT...\n";
     generateDOT( *project );

     cout << "Done with DOT\n";

     printf ("\nLeaving main program ... \n");
  // backend(project);

     return 0;
   }

