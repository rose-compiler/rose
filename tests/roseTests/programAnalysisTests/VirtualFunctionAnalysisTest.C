#include "sage3basic.h"

#include <StmtInfoCollect.h>
#include <AstInterface_ROSE.h>

#include <DefUseChain.h>
#include <string>
#include <iostream>
#include <CommandOptions.h>
#include <GraphIO.h>

#include "DataFlowAnalysis.h"
#include "BitVectorRepr.h"
#include "AnalysisInterface.h"
#include "VirtualFunctionAnalysis.h"
#include <GraphDotOutput.h>
#include <CallGraph.h>
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
    std::cout <<"Generating AST dot\n";
    generateDOT(*project);
    generateAstGraph(project, 20000);

    CallGraphBuilder builder(project);
    builder.buildCallGraph();
    // Generate call graph in dot format
    AstDOTGeneration dotgen;
    dotgen.writeIncidenceGraphToDOTFile(builder.getGraph(), "full_call_graph.dot");


    SageInterface::changeAllLoopBodiesToBlocks(project);

    SgFunctionDeclaration *mainDecl = SageInterface::findMain(project);
    if(mainDecl == NULL) {
            std::cerr<< "Can't execute Virtual Function Analysis without main function\n";
            return 0;
     }


    
    VirtualFunctionAnalysis *anal = new VirtualFunctionAnalysis(project);
    anal->run();
    
    
     anal->pruneCallGraph(builder);
     
     AstDOTGeneration dotgen2;
     dotgen2.writeIncidenceGraphToDOTFile(builder.getGraph(), "pruned_call_graph.dot");

    

    

    delete anal;
    
    
    return 0;
}


