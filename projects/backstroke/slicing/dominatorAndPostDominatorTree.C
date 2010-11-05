#include <rose.h>
#include "backstrokeCFG.h"
#include <boost/foreach.hpp>
#include <boost/graph/adjacency_list.hpp>
#include <boost/graph/graphviz.hpp>
#include <boost/graph/filtered_graph.hpp>
#include <boost/tuple/tuple.hpp>

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



class DominatorTreeBuilder
{
public:
    //typedef std::map<SgGraphNode*, set<SgGraphNode*> > NodeNodesMap;
    //typedef std::map<SgGraphNode*, SgGraphNode*> NodeNodeMap;
    
    typedef adjacency_list<vecS, vecS, bidirectionalS, 
            property<vertex_name_t, SgGraphNode*> > DominatorTree;

private:
    //! A map from each CFG node to its all dominators.
    std::map<SgGraphNode*, set<SgGraphNode*> > node_to_dominators_;

    //! A map from each CFG node to its immediate dominator.
    std::map<SgGraphNode*, SgGraphNode*> node_to_immediate_dominator_;

    //! The dominator tree represented by a boost::graph
    DominatorTree dominator_tree_;

    typedef graph_traits<DominatorTree>::vertex_descriptor Node;
    typedef property_map<DominatorTree, vertex_name_t>::type NodeSgNodeMap;

    //! A functor class helping write dot file.
    class NodeWriter 
    {
        const NodeSgNodeMap& node_sgnode_map_;
        public:
        NodeWriter(const NodeSgNodeMap& node_sgnode_map) : node_sgnode_map_(node_sgnode_map) {}

        void operator()(std::ostream& out, const Node& node) const 
        {
            VirtualCFG::CFGNode cfgnode(node_sgnode_map_[node]->get_SgNode(), StaticCFG::CFG::getIndex(node_sgnode_map_[node]));
            out << "[label=\"" << escapeString(cfgnode.toString()) << "\"]";
        }
    };


public:
    void build(const StaticCFG::CFG& cfg);

    std::map<SgGraphNode*, set<SgGraphNode*> >
    getNodeToDominatorsMap() const { return node_to_dominators_; }    

    std::map<SgGraphNode*, SgGraphNode*>
    getNodeToImmediateDominatorMap() const { return node_to_immediate_dominator_; }    

    const DominatorTree& getDominatorTree() const { return dominator_tree_; }

    void toDot(const std::string& filename) const;

};

void DominatorTreeBuilder::build(const StaticCFG::CFG& cfg)
{
    // Clear everything which will be built.
    node_to_dominators_.clear();
    node_to_immediate_dominator_.clear();
    dominator_tree_.clear();

	SgIncidenceDirectedGraph* graph = cfg.getGraph();

	// Get the entry node En.
	SgGraphNode* entry = cfg.getEntry();

	// Get all nodes N in the CFG.
	set<SgGraphNode*> all_nodes = graph->computeNodeSet();
	ROSE_ASSERT(all_nodes.count(entry) > 0);

	// First, initialize dominator(En) to {En}.
	node_to_dominators_[entry].insert(entry);

	// Initialize the dominator of other nodes to N.
	foreach (SgGraphNode* n, all_nodes)
	{
		if (n != entry)
			node_to_dominators_[n] = all_nodes;
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
				node_to_immediate_dominator_[nodes.first] = n;
				break;
			}
		}
	}
	ROSE_ASSERT(node_to_immediate_dominator_.size() == node_to_dominators_.size() - 1);

    // Start to build the dominator tree.
    NodeSgNodeMap node_to_sgnode = get(vertex_name, dominator_tree_);
    map<SgGraphNode*, Node> nodes_added;

    typedef map<SgGraphNode*, SgGraphNode*>::value_type NodeToNode;
    foreach (const NodeToNode& nodes, node_to_immediate_dominator_)
    {
        SgGraphNode *from, *to;
        Node src, tar;
        map<SgGraphNode*, Node>::iterator it;
        bool inserted;

        tie(from, to) = nodes;

        // Add the first node.
        tie(it, inserted) = nodes_added.insert(make_pair(from, tar));
        if (inserted)
        {
            tar = add_vertex(dominator_tree_);
            node_to_sgnode[tar] = from;
            it->second = tar;
        }
        else
            tar = it->second;


        // Add the second node.
        tie(it, inserted) = nodes_added.insert(make_pair(to, src));
        if (inserted)
        {
            src = add_vertex(dominator_tree_);
            node_to_sgnode[src] = to;
            it->second = src;
        }
        else
            src = it->second;

        // Add the edge.
        add_edge(src, tar, dominator_tree_);
    }

    ofstream out("dominatorTree.dot");
    write_graphviz(out, dominator_tree_, NodeWriter(node_to_sgnode));
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
  }

  return 0;
}