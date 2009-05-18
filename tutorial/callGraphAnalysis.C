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
#ifdef HAVE_SQLITE3
     var_SOLVE_FUNCTION_CALLS_IN_DB = true;
#endif

     CallGraphBuilder CGBuilder( project );
     CGBuilder.buildCallGraph();

  // Classify subgraphs within call graph
     cout << "Classifying...\n";
     CGBuilder.classifyCallGraph();
     cout << "Done classifying\n";

     GenerateDotGraph(CGBuilder.getGraph(),"callgraph.dot");

// DQ (7/28/2005): Added use of macro set in config.h so that call 
// graphs can be generated even if SQLite is not installed with ROSE.
// TPS (01Dec2008): Enabled mysql and this fails.
// seems like it is not supposed to be included
#ifdef HAVE_SQLITE3
     CallGraphDotOutput output( *(CGBuilder.getGraph()) );

     output.writeToDB( 1, "DATABASE" );

  // DQ (9/9/2005): Added "" as name for what the filter on
     output.filterNodesByFilename( "DATABASE","" );

     cout << "Loading from DB...\n";
     CallGraphCreate *newGraph = output.loadGraphFromDB( "DATABASE" );
     cout << "Loaded\n";
     ostringstream st;
     st << "DATABASE.dot";

  // GenerateDotGraph(newGraph, st.str());
     GenerateDotGraph( newGraph, "callgraphSQL.dot" );
#endif

     cout << "Generating DOT...\n";
     generateDOT( *project );

     cout << "Done with DOT\n";

     printf ("\nLeaving main program ... \n");
  // backend(project);

     return 0;
   }

