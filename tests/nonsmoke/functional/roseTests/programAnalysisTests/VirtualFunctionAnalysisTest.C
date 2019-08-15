#include "sage3basic.h"

#include <string>
#include <iostream>
#include <CommandOptions.h>
#include <CallGraph.h>

#include "VirtualFunctionAnalysis.h"
using namespace boost;

#define foreach BOOST_FOREACH
#define reverse_foreach BOOST_REVERSE_FOREACH

using namespace std;
using namespace boost;

void PrintUsage( char* name)
{
  cerr << name << " <options> " << "<program name>" << "\n";
  cerr << "-dot :generate DOT output \n";
}


int
main(int argc, char * argv[]) {

    if (argc <= 1) {
        PrintUsage(argv[0]);
        return -1;
    }
    SgProject* project = frontend(argc, argv);

    printf ("Build the call graph \n");

    CallGraphBuilder builder(project);
    builder.buildCallGraph();

    printf ("DONE: Build the call graph \n");

    // Generate call graph in dot format
    AstDOTGeneration dotgen;
    dotgen.writeIncidenceGraphToDOTFile(builder.getGraph(), "full_call_graph.dot");


    printf ("Calling SageInterface::changeAllBodiesToBlocks() \n");

    SageInterface::changeAllBodiesToBlocks(project);

    printf ("DONE: Calling SageInterface::changeAllBodiesToBlocks() \n");

    SgFunctionDeclaration *mainDecl = SageInterface::findMain(project);
    if(mainDecl == NULL) {
            std::cerr<< "Can't execute Virtual Function Analysis without main function\n";
            return 0;
     }
    
     printf ("Calling VirtualFunctionAnalysis() \n");

     VirtualFunctionAnalysis *anal = new VirtualFunctionAnalysis(project);

     printf ("Calling VirtualFunctionAnalysis(): run \n");

     anal->run();
    
     printf ("Calling VirtualFunctionAnalysis(): pruneCallGraph \n");
    
     anal->pruneCallGraph(builder);
     
#if 0     
     std::string filename;
     filename = string(strrchr(mainDecl->get_definition()->get_file_info()->get_filename() ,'/')+1);
     filename = filename + ".callGraph.dot";
#endif
     
     printf ("Calling VirtualFunctionAnalysis(): writeIncidenceGraphToDOTFile \n");

     AstDOTGeneration dotgen2;
     dotgen2.writeIncidenceGraphToDOTFile(builder.getGraph(), "call_graph.dot");
    
     printf ("Calling VirtualFunctionAnalysis(): delete \n");

     delete anal;
    
     printf ("DONE: Calling VirtualFunctionAnalysis(): delete \n");
    
    return 0;
}


