#include <rose.h>
#include "backstrokeCFG.h"
#include <boost/foreach.hpp>
#include <boost/graph/adjacency_list.hpp>
#include <boost/graph/graphviz.hpp>
#include <boost/graph/filtered_graph.hpp>
#include <boost/tuple/tuple.hpp>
#include <boost/bind.hpp>

using namespace std;
using namespace boost;


#define foreach BOOST_FOREACH


class DominatorTreeBuilder
{
public:
    //typedef std::map<SgGraphNode*, set<SgGraphNode*> > NodeNodesMap;
    //typedef std::map<SgGraphNode*, SgGraphNode*> NodeNodeMap;

	//typedef VirtualCFG::CFGNode CFGNode;
	typedef Backstroke::CFG::Vertex CFGVertex;
    typedef adjacency_list<vecS, vecS, directedS, CFGNode> DominatorTree;


private:
    //! The dominator tree represented by a boost::graph
    DominatorTree dominatorTree_;

    typedef graph_traits<DominatorTree>::vertex_descriptor Node;
    //typedef property_map<DominatorTree, vertex_name_t>::type NodeSgNodeMap;

	void writeTreeNode(std::ostream& out, const Node& node)
	{
		Backstroke::writeCFGNode(out, dominatorTree_[node]);
	}

public:
    void build(const Backstroke::CFG& cfg);

    const DominatorTree& getDominatorTree() const { return dominatorTree_; }

    void toDot(const std::string& filename) const;
};

void DominatorTreeBuilder::build(const Backstroke::CFG& cfg)
{
    // Clear everything which will be built.
    //node_to_dominators_.clear();
    //immediateDominators_.clear();
    dominatorTree_.clear();

#if 0
	//SgIncidenceDirectedGraph* graph = cfg.getGraph();

	// Get the entry node En.
	CFGVertex entry = cfg.getEntry();

	vertex_iterator i, j;

	// Get all nodes N in the CFG.
	set<CFGVertex> all_nodes;
	for (tie(i, j) = vertices(cfg); i != j; ++i)
		all_nodes.insert(*i);

	ROSE_ASSERT(all_nodes.count(entry) > 0);
	
	// First, initialize dominator(En) to {En}.
	node_to_dominators_[entry].insert(entry);
	
	// Initialize the dominator of other nodes to N.
	for (tie(i, j) = vertices(cfg); i != j; ++i)
	//foreach (SgGraphNode* n, all_nodes)
	{
		if (*i != entry)
			node_to_dominators_[*i] = all_nodes;
	}

	bool change = true;
	while(change)
	{
		change = false;

		foreach (CFGVertex n, all_nodes)
		{
			if (n != entry)
			{
				vector<SgGraphNode*> predecessors;

				predecessor_map()
				//graph->getPredecessors(n, predecessors);
				getPredecessors(graph, n, predecessors);
				ROSE_ASSERT(!predecessors.empty());

				// Here we get new dominators of n by intersecting all dominators of its predecessors,
				// plus itself.
				set<SgGraphNode*> new_dominators = node_to_dominators_[predecessors[0]];
				for (size_t i = 1; i < predecessors.size(); ++i)
				{
					set<SgGraphNode*> temp_dom;
					const set<SgGraphNode*>& p_dom = node_to_dominators_[predecessors[i]];
					set_intersection(new_dominators.begin(), new_dominators.end(),
							p_dom.begin(), p_dom.end(), inserter(temp_dom, temp_dom.begin()));
					new_dominators.swap(temp_dom);
				}
				new_dominators.insert(n);

				// If the old and new dominators are different, set the 'change' flag to true.
				set<SgGraphNode*>& old_dominators = node_to_dominators_[n];
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
	typedef map<SgGraphNode*, set<SgGraphNode*> >::value_type NodeToNodes;
	foreach (const NodeToNodes& nodes, node_to_dominators_)
	{
		foreach (SgGraphNode* n, nodes.second)
		{
			// If the size of dominator(d) for one dominator d of the node n equals to the
			// size of dominator(n) - 1, then d is the immediate dominator of n.
			if (node_to_dominators_[n].size() == nodes.second.size() - 1)
			{
				immediateDominators_[nodes.first] = n;
				break;
			}
		}
	}
	ROSE_ASSERT(immediateDominators_.size() == node_to_dominators_.size() - 1);
#endif

	map<CFGVertex, CFGVertex> immediateDominators_ = cfg.buildDominatorTree();

	cout << num_vertices(cfg) << endl;
	cout << immediateDominators_.size() << endl;

    // Start to build the dominator tree.
    //NodeSgNodeMap node_to_sgnode = get(vertex_name, dominatorTree_);
    map<CFGVertex, Node> nodesAdded;

    typedef map<CFGVertex, CFGVertex>::value_type NodeToNode;
    foreach (const NodeToNode& nodes, immediateDominators_)
    {
        CFGVertex from, to;
        Node src, tar;
        map<CFGVertex, Node>::iterator it;
        bool inserted;

        tie(from, to) = nodes;

        // Add the first node.
        tie(it, inserted) = nodesAdded.insert(make_pair(from, tar));
        if (inserted)
        {
            tar = add_vertex(dominatorTree_);
            dominatorTree_[tar] = cfg[from];
            it->second = tar;
        }
        else
            tar = it->second;


        // Add the second node.
        tie(it, inserted) = nodesAdded.insert(make_pair(to, src));
        if (inserted)
        {
            src = add_vertex(dominatorTree_);
			dominatorTree_[src] = cfg[to];
            it->second = src;
        }
        else
            src = it->second;

        // Add the edge.
        add_edge(src, tar, dominatorTree_);
    }

    ofstream out("dominatorTree.dot");
    write_graphviz(out, dominatorTree_, bind(&DominatorTreeBuilder::writeTreeNode, this, _1, _2));
}

#if 0
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

	//buildDominitorTree(cfg);
    DominatorTreeBuilder dom_tree_builder;
    dom_tree_builder.build(cfg);
  }

  return 0;
}
#endif

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

	Backstroke::CFG cfg(proc);
	cfg.toDot("temp.dot");

	DominatorTreeBuilder dom_tree_builder;
    dom_tree_builder.build(cfg);
  }

  return 0;
}