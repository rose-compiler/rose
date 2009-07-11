#include "rose.h"
#include <GraphAccess.h>
#include <GraphUpdate.h>
#include <CallGraph.h>

using namespace std;

#if 0
bool filter ( SgNode *n )
    {
   // false will filter out ALL nodes
      bool filterNode = true;
#if 0
      SgFunctionDeclaration *fct = isSgFunctionDeclaration( n );
      if ( fct != NULL )
         {
           bool ignoreFunction = (fct->get_file_info()->isCompilerGenerated() == true);
           if ( ignoreFunction == true )
                filterNode = false;
         }
#endif
      return filterNode;
    }
#endif


std::string edgeLabel(SgDirectedGraphEdge* edge)
{
    return "some label";
};

std::string nodeLabel(SgGraphNode* node)
{
    return "some label";
};


int
main( int argc, char * argv[] ) {
   RoseTestTranslator test;
   //  r = test.translate(argc,argv); // returns an error if any of the ends fails
   std::vector<std::string> argvList(argv, argv+argc);
   std::string dbName = "";
   CommandlineProcessing::isOptionWithParameter(argvList,"-db:","(name)", dbName,true);

   bool var_SOLVE_FUNCTION_CALLS_IN_DB = false;
//   var_SOLVE_FUNCTION_CALLS_IN_DB = true;
#ifdef HAVE_SQLITE3
   var_SOLVE_FUNCTION_CALLS_IN_DB = true;
   std::cout << "Analyzing in DATABASE" << std::endl;

   if(dbName == "")
   {
     std::cerr << "Error: Please specify a database name with the -db:name option" << std::endl;
     exit(1);
   }

   sqlite3x::sqlite3_connection* gDB = open_db(dbName);
#else
   std::cout << "Analyzing outside DATABASE" << std::endl;

#endif

   SgProject* project = new SgProject(argvList);

   CallGraphBuilder CGBuilder( project, var_SOLVE_FUNCTION_CALLS_IN_DB );
   CGBuilder.buildCallGraph();

   //  GenerateDotGraph(CGBuilder.getGraph(),"callgraph.dot");

   ClassHierarchyWrapper hier( project );

   // Use the information in the graph to output a dot file for the call graph

   //CallGraphDotOutput output( *(CGBuilder.getGraph()) );

// TPS (01Dec2008): Enabled mysql and this fails.
// seems like it is not supposed to be included
   SgIncidenceDirectedGraph *newGraph;
   if(var_SOLVE_FUNCTION_CALLS_IN_DB == true)
   {

#ifdef HAVE_SQLITE3
     writeSubgraphToDB( *gDB,  CGBuilder.getGraph() );

     hier.writeHierarchyToDB(*gDB);

     filterNodesByDirectory(*gDB, "/export" );
     solveVirtualFunctions(*gDB, "ClassHierarchy" );
     solveFunctionPointers( *gDB );
     cout << "Loading from DB...\n";

     std::vector<std::string> keepDirs;
     keepDirs.push_back( ROSE_COMPILE_TREE_PATH+std::string("%") );
     filterNodesKeepPaths(*gDB, keepDirs);

     std::vector<std::string> removeFunctions;
     removeFunctions.push_back("::main%" );
     filterNodesByFunctionName(*gDB,removeFunctions);

     newGraph = loadCallGraphFromDB(*gDB);
     cout << "Loaded\n";

#endif

   }else{
     // Not SQL Database case
     printf ("Not using the SQLite Database ... \n");
     newGraph = CGBuilder.getGraph();

     //filterGraph(*newGraph,filterNodes());

   }

   ostringstream st;
   st << "DATABASE.dot";
   cout << "Generating DOT...\n";
  
   OutputDot::writeToDOTFile(newGraph, st.str()+"2","Incidence Graph", nodeLabel,edgeLabel );
   generateDOT( *project );
   cout << "Done with DOT\n";
   GenerateDotGraph(newGraph, st.str());

   printf ("\nLeaving main program ... \n");

   return 0; // backend(project);
}
