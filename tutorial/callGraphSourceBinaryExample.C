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
  // Initialize and check compatibility. See rose::initialize
  ROSE_INITIALIZE;

  SgProject* project = frontend(argc,argv);
  ROSE_ASSERT (project != NULL);

  // create Call graph
  SB_CallGraph* callgraph = new SB_CallGraph();
  if (callgraph) {
    callgraph->createCallGraph(project);
    RoseBin_Graph* graph = callgraph->getBinaryCallGraph()->getGraph();
    if (graph) {
      rose_graph_integer_node_hash_map nodes = graph->get_node_index_to_node_map();//get_nodes()->get_nodes();
      rose_graph_integer_edge_hash_multimap edges = graph->get_node_index_to_edge_multimap_edgesOut();//get_edgesOut()->get_edges();
    	cerr << " Binary call graph found nodes : " << nodes.size() << "  found edges : " << edges.size() << endl;
    }
    SB_DirectedGraph* srcgraph = callgraph->getSourceCallGraph()->getGraph();
	if (srcgraph) {
	  rose_graph_integer_node_hash_map nodes = srcgraph->get_node_index_to_node_map();//get_nodes()->get_nodes();
	  rose_graph_integer_edge_hash_multimap edges = srcgraph->get_node_index_to_edge_multimap_edgesOut();//get_edgesOut()->get_edges();
		cerr << " Source call graph found nodes : " << nodes.size() << "  found edges : " << edges.size() << endl;
	}
  }
  // Unparse the output to test the unparser...
  return backend(project);
}

