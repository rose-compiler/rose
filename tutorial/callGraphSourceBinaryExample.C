/****************************************************
 * Tutorial example on how to use the call graph for
 * source code and binary
 * Author : tps
 * Date : 27Apr2009
 ****************************************************/

#include "rose.h"
#include "RoseSrc_CallGraphAnalysis.h"
#include "CallGraphAnalysis.h"

using namespace std;



int main(int argc, char** argv)
{
  SgProject* project = frontend(argc,argv);
  ROSE_ASSERT (project != NULL);

  // create Call graph
  SB_CallGraph* callgraph = new SB_CallGraph();
  if (callgraph) {
    callgraph->createCallGraph(project);
    RoseBin_Graph* graph = callgraph->getBinaryCallGraph()->getGraph();
    if (graph) {
    	SB_DirectedGraph::nodeType nodes = graph->nodes;
    	rose_hash::hash_multimap < SgDirectedGraphNode*, SgDirectedGraphEdge*> edges = graph->edges;
    	cerr << " Binary call graph found nodes : " << nodes.size() << "  found edges : " << edges.size() << endl;
    }
    SB_DirectedGraph* srcgraph = callgraph->getSourceCallGraph()->getGraph();
	if (srcgraph) {
		SB_DirectedGraph::nodeType nodes = srcgraph->nodes;
		rose_hash::hash_multimap < SgDirectedGraphNode*, SgDirectedGraphEdge*> edges = srcgraph->edges;
		cerr << " Source call graph found nodes : " << nodes.size() << "  found edges : " << edges.size() << endl;
	}
  }
  // Unparse the output to test the unparser...
  return backend(project);
}

