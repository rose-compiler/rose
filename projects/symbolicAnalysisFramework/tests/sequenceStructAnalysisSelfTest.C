#include "rose.h"
#include <list>
#include <sstream>
#include <iostream>
#include <fstream>
#include <string.h>
using namespace std;

#include "common.h"
#include "variables.h"
#include "cfgUtils.h"
#include "analysisCommon.h"
#include "functionState.h"
#include "latticeFull.h"
#include "analysis.h"
#include "dataflow.h"
#include "liveDeadVarAnalysis.h"
#include "divAnalysis.h"
#include "saveDotAnalysis.h"
#include "sequenceStructAnalysis.h"

int numFails=0;

int main( int argc, char * argv[] ) 
{
        printf("========== S T A R T ==========\n");
        
        // Build the AST used by ROSE
        SgProject* project = frontend(argc,argv);
        
        initAnalysis(project);
        
        analysisDebugLevel = 0;
        
        CallGraphBuilder cgb(project);
        cgb.buildCallGraph();
        SgIncidenceDirectedGraph* graph = cgb.getGraph(); 
        
        /*SaveDotAnalysis sda;
        UnstructuredPassInterAnalysis upia_sda(sda);
        upia_sda.runAnalysis();*/
        
        analysisDebugLevel = 0;
        printf("*************************************************************\n");
        printf("*****************   Live/Dead Variable Analysis   *****************\n");
        printf("*************************************************************\n");
        LiveDeadVarsAnalysis ldva(project);
        //CallGraphBuilder cgb(project);
        //cgb.buildCallGraph();
        //SgIncidenceDirectedGraph* graph = cgb.getGraph(); 
        //ContextInsensitiveInterProceduralDataflow ciipd_da(&ldva, graph);
        UnstructuredPassInterDataflow ciipd_ldva(&ldva);
        ciipd_ldva.runAnalysis();
        
        //printLiveDeadVarsAnalysisStates(&ldva, "[");
        
        analysisDebugLevel = 2;
        divAnalysisDebugLevel = 1;
        printf("*************************************************************\n");
        printf("*****************   Divisibility Analysis   *****************\n");
        printf("*************************************************************\n");
        DivAnalysis da(&ldva);
        //ContextInsensitiveInterProceduralDataflow ciipd_da(&da, graph);
        UnstructuredPassInterDataflow ciipd_da(&da);
        ciipd_da.runAnalysis();
        
        //printDivAnalysisStates(&da, "[");
        
        analysisDebugLevel = 1;
        sequenceStructAnalysisDebugLevel = 1;
        printf("***************************************************************\n");
        printf("***************** Sequence Structure Analysis *****************\n");
        printf("***************************************************************\n");
        SeqStructAnalysis ssa(&ldva, &da);
        //ContextInsensitiveInterProceduralDataflow ciipd_ssa(&ssa, graph);
        UnstructuredPassInterDataflow ciipd_ssa(&ssa);
        ciipd_ssa.runAnalysis();
        
        printSeqStructAnalysisStates(&ssa, "<");

        if(numFails==0)
                printf("PASS\n");
        else
                printf("FAIL!\n");
        
        printf("==========  E  N  D  ==========\n");
        
        // Unparse and compile the project (so this can be used for testing)
        return backend(project);
}
