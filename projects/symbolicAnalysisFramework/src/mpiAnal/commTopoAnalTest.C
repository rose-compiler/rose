/*
 * Generic path-sensitive analysis that uses the PartitionedAnalysis framework to create a 
 * separate intra-procedural analysis for every if statement in the function
 */

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
#include "divAnalysis.h"
#include "sgnAnalysis.h"
//#include "intArithLogical.h"
#include "saveDotAnalysis.h"
#include "partitionedAnalysis.h"
#include "contProcMatchAnalysis.h"
#include "rankDepAnalysis.h"
#include "ranknpDepIfMeet.h"

extern int MPIRankDepAnalysisDebugLevel;
int numFails=0;

int main( int argc, char * argv[] ) 
{
	printf("========== S T A R T ==========\n");
	
	// Build the AST used by ROSE
	SgProject* project = frontend(argc,argv);
	
	cfgUtils::initCFGUtils(project);

	initAnalysis(project);
	
	analysisDebugLevel = 0;
	
	/*SaveDotAnalysis sda;
	UnstructuredPassInterAnalysis upia_sda(sda);
	upia_sda.runAnalysis();
	
	CallGraphBuilder cgb(project);
	cgb.buildCallGraph();
	SgIncidenceDirectedGraph* graph = cgb.getGraph(); 
	*/
	
	analysisDebugLevel = 0;
	printf("*************************************************************\n");
	printf("*****************   Divisibility Analysis   *****************\n");
	printf("*************************************************************\n");
	DivAnalysis da;
	//ContextInsensitiveInterProceduralDataflow ciipd_da(&da, graph);
	UnstructuredPassInterDataflow ciipd_da(&da);
	ciipd_da.runAnalysis();
	
	//printDivAnalysisStates(&da, "[");
	
	analysisDebugLevel = 0;
	
	printf("*************************************************************\n");
	printf("*****************   Sign Analysis   *****************\n");
	printf("*************************************************************\n");
	SgnAnalysis sa;
	//ContextInsensitiveInterProceduralDataflow ciipd_sa(&sa, graph);
	UnstructuredPassInterDataflow ciipd_sa(&sa);
	ciipd_sa.runAnalysis();
	
	//printSgnAnalysisStates(&sa, "|");
	
	analysisDebugLevel = 0;
	
	printf("*************************************************************\n");
	printf("******************   Affine Inequalities   ******************\n");
	printf("*************************************************************\n");
	runAffineIneqPlacer(false);
	
	/*analysisDebugLevel = 1;
	
	printf("*************************************************************\n");
	printf("**********   Integer-Arithmetic-Logical Expressions *********\n");
	printf("*************************************************************\n");
	runIntArithLogicalPlacer(true);*/
	
	analysisDebugLevel = 0;
	MPIRankDepAnalysisDebugLevel = 0;
	printf("*************************************************************\n");
	printf("***************   MPI Rank Dependence Analysis **************\n");
	printf("*************************************************************\n");
	runMPIRankDepAnalysis();
	
	/*analysisDebugLevel = 0;
	
	printf("*************************************************************\n");
	printf("************** MPI Rank IfMeetDetector Analysis *************\n");
	printf("*************************************************************\n");
	
	runRankDepIfMeetDetector(false);*/
	
	analysisDebugLevel = 0;
	
	printf("*************************************************************\n");
	printf("**************     IfMeetDetector Analysis      *************\n");
	printf("*************************************************************\n");
	
	runIfMeetDetector(false);
	
	analysisDebugLevel = 0;
	
	printf("***************************************************************\n");
	printf("********** Communication Topology Dataflow Analysis ***********\n");
	printf("***************************************************************\n");
	/*commTopoPartitionedAnalysis sfwpa(&da, &sa);
	UnstructuredPassInterAnalysis upia_sfwpa(sfwpa);
	upia_sfwpa.runAnalysis();*/
	pCFG_contProcMatchAnalysis sfwpa(&da, &sa);
	UnstructuredPassInterAnalysis upia_sfwpa(sfwpa);
	upia_sfwpa.runAnalysis();
	
	printf("*********************************************************\n");
	printf("***************** Final Analysis States *****************\n");
	printf("*********************************************************\n");
	
	vector<int> factNames;
	vector<int> latticeNames;
	latticeNames.push_back(0);
	//printf("Master Analysis = %p\n", sfwpa.getMasterDFAnalysis());
	//printAnalysisStates pas(sfwpa.getMasterDFAnalysis(), factNames, latticeNames, ":");
	printAnalysisStates pas(&sfwpa, factNames, latticeNames, ":");
	UnstructuredPassInterAnalysis upia_pas(pas);
	upia_pas.runAnalysis();

	if(numFails==0)
		printf("PASS\n");
	else
		printf("FAIL!\n");
	
	printf("==========  E  N  D  ==========\n");
	
	// Unparse and compile the project (so this can be used for testing)
	return backend(project);
}
