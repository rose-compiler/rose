#include "rose.h"
#include <CallGraph.h>
#include <GraphUpdate.h>
using namespace std;

//Function object that determines which functions are to be represented
//in the call graph.
struct keepFunction : public unary_function<bool,SgFunctionDeclaration*>{
    public:
      bool operator()(SgFunctionDeclaration* funcDecl){
        bool returnValue = true;

        ROSE_ASSERT(funcDecl != NULL);
        string filename = funcDecl->get_file_info()->get_filename();

        //Filter out functions from the ROSE preinclude header file
        if(filename.find("rose_edg_required_macros_and_functions")!=string::npos)
          returnValue = false;
        //Filter out compiler generated functions
        if(funcDecl->get_file_info()->isCompilerGenerated()==true)
          returnValue=false;

        return returnValue;
      }
};

int
main( int argc, char * argv[] ) {
   RoseTestTranslator test;
   SgProject* project = new SgProject(argc, argv);

   //Construct Call Graph
   CallGraphBuilder CGBuilder( project);
   CGBuilder.buildCallGraph(keepFunction());

   cout << "Generating DOT...\n";
   generateDOT( *project );
   cout << "Done with DOT\n";

   // Use the information in the graph to output a dot file for the call graph
   AstDOTGeneration dotgen;
   dotgen.writeIncidenceGraphToDOTFile( CGBuilder.getGraph() , "callGraph.dot");

   printf ("\nLeaving main program ... \n");
   return 0; // backend(project);
}
