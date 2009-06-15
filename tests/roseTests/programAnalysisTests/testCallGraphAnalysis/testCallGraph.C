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

struct filterNodes : public unary_function<CallGraphNode*,bool>{
    public:
      bool operator()(CallGraphNode* test){

              bool returnValue = false;
              SgFunctionDeclaration* CallGraphNode2 = test->functionDeclaration;

              string filename = CallGraphNode2->get_file_info()->get_filename();
              if( filename.find("g++_HEADERS")!=string::npos ||
                  filename.find("/usr/include")!=string::npos){
//               std::cout << "Filter out STL headers " << test->toString()<<std::endl;
                returnValue= true;
              }
              if(test->toString().find(string("_"))!=string::npos)
                returnValue = true;
              if(filename.find("rose_edg_macros_and_functions_required_for_gnu.h")!=string::npos)
                 returnValue = true;
         if(CallGraphNode2->get_file_info()->isCompilerGenerated()==true)
            returnValue=true;

         return returnValue;
      }
};



int
main( int argc, char * argv[] ) {
   RoseTestTranslator test;
   //  r = test.translate(argc,argv); // returns an error if any of the ends fails
   SgProject* project = new SgProject(argc, argv);

//   var_SOLVE_FUNCTION_CALLS_IN_DB = true;
#ifdef HAVE_SQLITE3
   var_SOLVE_FUNCTION_CALLS_IN_DB = true;
   std::cout << "Analyzing in DATABASE" << std::endl;
   sqlite3x::sqlite3_connection* gDB = open_db("DATABASE");
#else
   std::cout << "Analyzing outside DATABASE" << std::endl;

#endif

   CallGraphBuilder CGBuilder( project );
   CGBuilder.buildCallGraph();

   // Classify subgraphs within call graph
   cout << "Classifying...\n";
   CGBuilder.classifyCallGraph();
   cout << "Done classifying\n";
   //  GenerateDotGraph(CGBuilder.getGraph(),"callgraph.dot");

   ClassHierarchyWrapper hier( project );

   // Use the information in the graph to output a dot file for the call graph

   CallGraphDotOutput output( *(CGBuilder.getGraph()) );

// TPS (01Dec2008): Enabled mysql and this fails.
// seems like it is not supposed to be included
   CallGraphCreate *newGraph;
   if(var_SOLVE_FUNCTION_CALLS_IN_DB == true)
   {

#ifdef HAVE_SQLITE3
     output.writeSubgraphToDB( *gDB );

     hier.writeHierarchyToDB(*gDB);

     output.filterNodesByDirectory(*gDB, "/export" );
     output.filterNodesByDB( *gDB, "__filter.db" );
     output.solveVirtualFunctions(*gDB, "ClassHierarchy" );
     output.solveFunctionPointers( *gDB );
     cout << "Loading from DB...\n";

     std::vector<std::string> keepDirs;
     keepDirs.push_back( ROSE_COMPILE_TREE_PATH+std::string("%") );
     filterNodesKeepPaths(*gDB, keepDirs);

     std::vector<std::string> removeFunctions;
     removeFunctions.push_back("::main%" );
     filterNodesByFunctionName(*gDB,removeFunctions);

     newGraph = output.loadGraphFromDB( *gDB);
     cout << "Loaded\n";

#endif

   }else{
     // Not SQL Database case
     printf ("Not using the SQLite Database ... \n");
     newGraph = CGBuilder.getGraph();

     filterGraph(*newGraph,filterNodes());

   }

   ostringstream st;
   st << "DATABASE.dot";
   cout << "Generating DOT...\n";


   generateDOT( *project );
   cout << "Done with DOT\n";
   GenerateDotGraph(newGraph, st.str());

   printf ("\nLeaving main program ... \n");

   return 0; // backend(project);
}
