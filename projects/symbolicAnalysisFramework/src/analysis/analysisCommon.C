#include "analysisCommon.h"

using namespace cfgUtils;

//static SgProject* project;
static SgIncidenceDirectedGraph* callGraph;

void initAnalysis(SgProject* project)
{
	//project = p;
	initCFGUtils(project);
	
	// Create the Call Graph
	CallGraphBuilder cgb(project);
	cgb.buildCallGraph();
	callGraph = cgb.getGraph(); 
	//GenerateDotGraph(graph, "test_example.callgraph.dot");
}

/*SgProject* getProject()
{
	return project;
}
*/
SgIncidenceDirectedGraph* getCallGraph()
{
	return callGraph;
}
