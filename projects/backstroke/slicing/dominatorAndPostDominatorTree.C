#include <rose.h>
#include <boost/foreach.hpp>

using namespace std;
using namespace boost;

#define foreach BOOST_FOREACH

void
getPredecessors(SgIncidenceDirectedGraph* graph, SgGraphNode* node, std::vector <SgGraphNode*>& vec ) {
  //SgGraphEdgeList* gredges = get_edgesIn();
  rose_graph_integer_edge_hash_multimap edges = graph->get_node_index_to_edge_multimap_edgesIn();
  rose_graph_integer_edge_hash_multimap::iterator it1, it2;
  pair <rose_graph_integer_edge_hash_multimap::iterator, rose_graph_integer_edge_hash_multimap::iterator> iter =
    //fails
    //	  get_edgesIn()->get_edges().equal_range(node);
    edges.equal_range(node->get_index());
  it1 = iter.first;
  it2 = iter.second;
  for (;it1!=it2; ++it1) {
    SgDirectedGraphEdge* edge = isSgDirectedGraphEdge(it1->second);
    if (edge) {
      SgGraphNode* source = isSgGraphNode(edge->get_from());
      if (source) {
	string type_n = graph->getProperty(SgGraph::type, edge);

	//if (type_n==RoseBin_support::ToString(SgGraph::cfg))
	  vec.push_back(source);
      }
    }
  }
}

map<SgGraphNode*, SgGraphNode*>
buildDominitorTree(const StaticCFG::CFG& cfg)
{
	SgIncidenceDirectedGraph* graph = cfg.getGraph();

	// Get the entry node En.
	SgGraphNode* entry = cfg.getEntry();

	// Get all nodes N in the CFG.
	set<SgGraphNode*> all_nodes = graph->computeNodeSet();

	ROSE_ASSERT(all_nodes.count(entry) > 0);

	// This variable maps every node to its dominators.
	map<SgGraphNode*, set<SgGraphNode*> > dominators;

	// First, initialize dominator(En) to {En}.
	dominators[entry].insert(entry);

	// Initialize the dominator of other nodes to N.
	foreach (SgGraphNode* n, all_nodes)
	{
		if (n != entry)
			dominators[n] = all_nodes;
	}

	bool change = true;
	while(change)
	{
		change = false;

		foreach (SgGraphNode* n, all_nodes)
		{
			if (n != entry)
			{
				vector<SgGraphNode*> predecessors;
				//graph->getPredecessors(n, predecessors);
				getPredecessors(graph, n, predecessors);
				ROSE_ASSERT(!predecessors.empty());

				// Here we get new dominators of n by intersecting all dominators of its predecessors,
				// plus itself.
				set<SgGraphNode*> new_dominators = dominators[predecessors[0]];
				for (size_t i = 1; i < predecessors.size(); ++i)
				{
					set<SgGraphNode*> temp_dom;
					const set<SgGraphNode*>& p_dom = dominators[predecessors[i]];
					set_intersection(new_dominators.begin(), new_dominators.end(),
							p_dom.begin(), p_dom.end(), inserter(temp_dom, temp_dom.begin()));
					new_dominators.swap(temp_dom);
				}
				new_dominators.insert(n);

				// If the old and new dominators are different, set the 'change' flag to true.
				set<SgGraphNode*>& old_dominators = dominators[n];
				if (old_dominators != new_dominators)
				{
					old_dominators.swap(new_dominators);
					change = true;
				}
			}
		}
	}

	// For each node, find its immediate dominator.
	map<SgGraphNode*, SgGraphNode*> immediate_dominator;
	typedef map<SgGraphNode*, set<SgGraphNode*> >::value_type T;
	foreach (const T& nodes, dominators)
	{
		cout << nodes.first->get_SgNode()->class_name() << "\n\t";
		foreach (SgGraphNode* n, nodes.second)
		{
			// If the size of dominator(d) for one dominator d of the node n equals to the
			// size of dominator(n) - 1, then d is the immediate dominator of n.
			if (dominators[n].size() == nodes.second.size() - 1)
			{
				immediate_dominator[nodes.first] = n;
				cout << n->get_SgNode()->class_name() << endl;
				break;
			}
		}
	}
	cout << immediate_dominator.size() << endl;
	cout << dominators.size() << endl;
	ROSE_ASSERT(immediate_dominator.size() == dominators.size() - 1);

	return immediate_dominator;
}

int main(int argc, char *argv[])
{
  // Build the AST used by ROSE
  SgProject* sageProject = frontend(argc,argv);

  // Process all function definition bodies for static control flow graph generation
  Rose_STL_Container<SgNode*> functions = NodeQuery::querySubTree(sageProject, V_SgFunctionDefinition);
  for (Rose_STL_Container<SgNode*>::const_iterator i = functions.begin(); i != functions.end(); ++i)
  {
    SgFunctionDefinition* proc = isSgFunctionDefinition(*i);
    ROSE_ASSERT (proc != NULL);
    string fileName= StringUtility::stripPathFromFileName(proc->get_file_info()->get_filenameString());
    string dotFileName1=fileName+"."+ proc->get_declaration()->get_name() +".debug.dot";
    string dotFileName2=fileName+"."+ proc->get_declaration()->get_name() +".interesting.dot";

    StaticCFG::CFG cfg(proc);

    // Dump out the full CFG, including bookkeeping nodes
    cfg.buildFullCFG();
    cfg.cfgToDot(proc, dotFileName1);

    // Dump out only those nodes which are "interesting" for analyses
    cfg.buildFilteredCFG();
    cfg.cfgToDot(proc, dotFileName2);

	buildDominitorTree(cfg);
  }

  return 0;
}

