#include "rose.h"
#include <list>
#include <sstream>
#include <iostream>
#include <fstream>
#include <string.h>
using namespace std;

#include "common.h"
#include "unionFind.h"
#include "variables.h"
#include "cfgUtils.h"
#include "analysisCommon.h"
#include "functionState.h"
#include "latticeFull.h"
#include "analysis.h"
#include "dataflow.h"
#include "divAnalysis.h"
#include "sgnAnalysis.h"
#include "affineInequality.h"
#include "chkptRangeAnalysis.h"
#include "saveDotAnalysis.h"
//#include "CallGraphTraverse.h"
//#include "CallGraphTraverse.C"
//#include "startSaveAnalysis.h"
//#include "rwAccessLabeler.h"
//#include "arrIndexLabeler.h"
//#include "CallGraph.h"
//#include "ompUtils.h"
//#include "ompNormalForm.h"

int numFails=0;


int main( int argc, char * argv[] ) 
{
	printf("========== S T A R T ==========\n");
	
	// Build the AST used by ROSE
	SgProject* project = frontend(argc,argv);
	
	cfgUtils::initCFGUtils(project);

	initAnalysis(project);
	
	analysisDebugLevel = 0;
	
	SaveDotAnalysis sda;
	UnstructuredPassInterAnalysis upia_sda(sda);
	upia_sda.runAnalysis();
	
	//analysisDebugLevel = 2;
	printf("*************************************************************\n");
	printf("*****************   Divisibility Analysis   *****************\n");
	printf("*************************************************************\n");
	DivAnalysis da;
	CallGraphBuilder cgb(project);
	cgb.buildCallGraph();
	SgIncidenceDirectedGraph* graph = cgb.getGraph(); 
	//ContextInsensitiveInterProceduralDataflow ciipd_da(&da, graph);
	UnstructuredPassInterDataflow ciipd_da(&da);
	ciipd_da.runAnalysis();
	
	analysisDebugLevel = 0;
	
	printf("*************************************************************\n");
	printf("*****************   Sign Analysis   *****************\n");
	printf("*************************************************************\n");
	SgnAnalysis sa;
	//ContextInsensitiveInterProceduralDataflow ciipd_sa(&sa, graph);
	UnstructuredPassInterDataflow ciipd_sa(&sa);
	ciipd_sa.runAnalysis();
	
	analysisDebugLevel = 2;
	
	printf("*************************************************************\n");
	printf("******************   Affine Inequalities   ******************\n");
	printf("*************************************************************\n");
	//initAffineIneqs(project);
	affineInequalitiesPlacer aip;
	UnstructuredPassInterAnalysis upia_aip(aip);
	upia_aip.runAnalysis();
	
	/*printAffineInequalities pai(&aip);
	UnstructuredPassInterAnalysis upia_pai(pai);
	upia_pai.runAnalysis();*/
	
	printf("*************************************************************\n");
	printf("***************** Checkpoint Range Analysis *****************\n");
	printf("*************************************************************\n");
	ChkptRangeAnalysis cra(&da, &sa, &aip);
	UnstructuredPassInterDataflow upid_cra(&cra);
	upid_cra.runAnalysis();
	
	printf("*************************************************************\n");
	printf("*********** Checkpoint Range Verification Analysis **********\n");
	printf("*************************************************************\n");
	ChkptRangeVerifAnalysis crva(&cra);
	UnstructuredPassInterAnalysis upia_crva(crva);
	upia_crva.runAnalysis();

	if(numFails==0)
		printf("PASS\n");
	else
		printf("FAIL!\n");
	
	printf("==========  E  N  D  ==========\n");
	
	// Unparse and compile the project (so this can be used for testing)
	return backend(project);
}
