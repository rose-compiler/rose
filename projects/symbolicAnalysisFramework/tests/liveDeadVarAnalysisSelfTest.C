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
#include "saveDotAnalysis.h"

int numFails=0;

int main( int argc, char * argv[] ) 
{
	printf("========== S T A R T ==========\n");
	
	// Build the AST used by ROSE
	SgProject* project = frontend(argc,argv);
	
	//cfgUtils::initCFGUtils(project);

	initAnalysis(project);
	
	analysisDebugLevel = 0;
	
	SaveDotAnalysis sda;
	UnstructuredPassInterAnalysis upia_sda(sda);
	upia_sda.runAnalysis();
	
	analysisDebugLevel = 1;
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
	
	printLiveDeadVarsAnalysisStates(&ldva, "[");

	Rose_STL_Container<SgFunctionCallExp*> funcCalls = NodeQuery::querySubTree(project, V_SgFunctionCallExp);
	for(Rose_STL_Container<SgFunctionCallExp*>::iterator f=funcCalls.begin(); f!=funcCalls.end(); f++) {
		cout << "<"<<(*f)->unparseToString()<<" | "<<(*f)->class_name()<<">\n";
	}

	if(numFails==0)
		printf("PASS\n");
	else
		printf("FAIL!\n");
	
	printf("==========  E  N  D  ==========\n");
	
	// Unparse and compile the project (so this can be used for testing)
	return backend(project);
}
