#include <list>
#include <sstream>
#include <iostream>
#include <fstream>
#include <string.h>
#include "genericDataflowCommon.h"
#include "variables.h"
#include "cfgUtils.h"
#include "analysisCommon.h"
#include "functionState.h"
#include "latticeFull.h"
#include "analysis.h"
#include "dataflow.h"
#include "constantPropagation.h"
#include "taintAnalysis.h"

int main( int argc, char * argv[] ) 
{
    printf("========== S T A R T ==========\n");
        
    // Build the AST used by ROSE
    SgProject* project = frontend(argc,argv);
        
    initAnalysis(project);
    Dbg::init("Taint Analysis", ".", "index.html");   
        
    analysisDebugLevel = 2;

    LiveDeadVarsAnalysis ldva(project);
    UnstructuredPassInterDataflow upid_ldva(&ldva);
    upid_ldva.runAnalysis();

    CallGraphBuilder cgb(project);
    cgb.buildCallGraph();
    SgIncidenceDirectedGraph* graph = cgb.getGraph();    

    SecureFunctionTypeTraversal sftype_traversal;
    sftype_traversal.traverse(project, preorder);

    // SecureFunctionTypeTraversalTest sftype_test;
    // sftype_test.traverse(project, preorder);

    TaintAnalysis ta(&ldva);
    ContextInsensitiveInterProceduralDataflow ciipd_ta(&ta, graph);
    ciipd_ta.runAnalysis();    

    return 0;
}
