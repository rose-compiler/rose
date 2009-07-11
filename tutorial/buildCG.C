#include "rose.h"
#include <CallGraph.h>
#include <GraphUpdate.h>
using namespace std;

struct filterNodes : public unary_function<bool,SgFunctionDeclaration*>{
    public:
      bool operator()(SgFunctionDeclaration* CallGraphNode2){
              bool returnValue = false;
              ROSE_ASSERT(CallGraphNode2 != NULL);
              string filename = CallGraphNode2->get_file_info()->get_filename();
              if( filename.find("g++_HEADERS")!=string::npos ||
                  filename.find("/usr/include")!=string::npos){
                returnValue= true;
              }
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
   SgProject* project = new SgProject(argc, argv);
   CallGraphBuilder CGBuilder( project, false /* Do not solve in the database */ );
   CGBuilder.buildCallGraph(filterNodes());

  //  GenerateDotGraph(CGBuilder.getGraph(),"callgraph.dot");

   ClassHierarchyWrapper hier( project );
// TPS (01Dec2008): Enabled mysql and this fails.
// seems like it is not supposed to be included
#if 0
   //#ifdef HAVE_MYSQL
   hier.setDBName( "ClassHierarchy" );
   hier.createHierarchySchema();
   hier.writeHierarchyToDB();
#endif
   // Use the information in the graph to output a dot file for the call graph
   //CallGraphDotOutput output( *(CGBuilder.getGraph()) );

// TPS (01Dec2008): Enabled mysql and this fails.
// seems like it is not supposed to be included
#if 0
   //#ifdef HAVE_MYSQL
   output.writeToDB( 1, "DATABASE" );
   output.filterNodesByDirectory( "DATABASE", "/export" );
   output.filterNodesByDB( "DATABASE", "__filter.db" );

#ifdef SOLVE_FUNCTION_CALLS_IN_DB
   output.solveVirtualFunctions( "DATABASE", "ClassHierarchy" );
   output.solveFunctionPointers( "DATABASE" );
#endif // SOLVE...

   cout << "Loading from DB...\n";
   CallGraphCreate *newGraph = output.loadGraphFromDB( "DATABASE" );
   cout << "Loaded\n";
#else
   // Not SQL Database case
   printf ("Not using the SQLite Database ... \n");
   SgIncidenceDirectedGraph *newGraph = CGBuilder.getGraph();
#endif // USE_ROSE...

   ostringstream st;
   st << "DATABASE.dot";
   cout << "Generating DOT...\n";
   //filterGraph(*newGraph,filterNodes());
   generateDOT( *project );
   cout << "Done with DOT\n";
   GenerateDotGraph(newGraph, st.str());
   printf ("\nLeaving main program ... \n");
   return 0; // backend(project);
}
