#ifndef BACKSTROKE_CFG_H
#define	BACKSTROKE_CFG_H


#include <rose.h>
#include <boost/graph/adjacency_list.hpp>
#include <boost/bind.hpp>
#include <boost/foreach.hpp>
#include <boost/graph/graphviz.hpp>
#include <boost/graph/dominator_tree.hpp>
#include <boost/graph/reverse_graph.hpp>
#include <boost/tuple/tuple.hpp>

#define foreach BOOST_FOREACH

namespace Backstroke
{


//typedef VirtualCFG::CFGNode CFGNode;
//typedef VirtualCFG::CFGEdge CFGEdge;

struct FullCFGType
{
	typedef VirtualCFG::CFGNode CFGNodeType;
	typedef VirtualCFG::CFGEdge CFGEdgeType;
};

struct FilteredCFGType
{
	typedef VirtualCFG::InterestingNode CFGNodeType;
	typedef VirtualCFG::InterestingEdge CFGEdgeType;
};

template <class CFGType>
class CFG : public boost::adjacency_list<boost::vecS, boost::vecS, boost::bidirectionalS, 
		typename CFGType::CFGNodeType, typename CFGType::CFGEdgeType>
{
public:
	typedef typename CFGType::CFGNodeType CFGNode;
	typedef typename CFGType::CFGEdgeType CFGEdge;
	//typedef VirtualCFG::InterestingEdge InterestingEdge;

	//typedef boost::property<vertex_all_t, CFGNode> CFGNodeProperty;
	//typedef boost::property<CFGNodeAttributeT, CFGEdge*,
	//        boost::property<InterestingEdgeAttributeT, InterestingEdge*> > CFGEdgeProperty;

	//typedef boost::adjacency_list<boost::vecS, boost::vecS, boost::directedS, CFGNode > Graph;

	typedef typename boost::graph_traits<CFG>::vertex_descriptor Vertex;
	typedef typename boost::graph_traits<CFG>::edge_descriptor Edge;

	typedef std::map<Vertex, Vertex> VertexToVertexMap;


protected:
	//typedef boost::property_map<Graph, GraphNodeSgNodePropertyT>::type NodePropertyMap;
	//typedef boost::property_map<Graph, CFGEdgeAttributeT>::type CFGEdgeAttributeMap;
	//typedef boost::property_map<Graph, InterestingEdgeAttributeT>::type InterestingEdgeAttributeMap;

	//! The graph data structure holding the CFG.
	//Graph graph_;

	//NodePropertyMap node_property_map_;


	////! A map from CFGNode in virtualCFG to node from staticCFG.
	//std::map<CFGNode, SgGraphNode*> all_nodes_;

	//! The entry node.
	Vertex entry_;

	//! The exit node.
	Vertex exit_;

	////! A flag shows whether this CFG is filtered or not.
	//bool is_filtered_;

public:

	CFG()
	:	entry_(boost::graph_traits<CFG>::null_vertex()),
		exit_(boost::graph_traits<CFG>::null_vertex())
	{
	}

	//! The constructor building the CFG.

	/*! The valid nodes are SgProject, SgStatement, SgExpression and SgInitializedName. */
	CFG(SgFunctionDefinition* funcDef)
	:	entry_(boost::graph_traits<CFG>::null_vertex()),
		exit_(boost::graph_traits<CFG>::null_vertex())
	{
		//node_property_map_ = boost::get(GraphNodeSgNodePropertyT, graph_);
		//edge_property_map_ = boost::get(CFGEdgeAttribute, graph_);
		//interesting_edge_property_map_ = boost::get(InterestingEdgeAttribute, graph_);

		build(funcDef);
	}

	//! Get the pointer pointing to the graph used by static CFG.

	//const Graph& getGraph() const
	//{
	//	return graph_;
	//}

	virtual ~CFG()
	{
	}

	void build(SgFunctionDefinition* funcDef);

	//! Get the entry node of the CFG
	const Vertex& getEntry() const
	{ return entry_; }

	//! Get the exit node of the CFG
	const Vertex& getExit() const
	{ return exit_; }

	VertexToVertexMap buildDominatorTree() const;

	VertexToVertexMap buildPostdominatorTree() const;
	
	//! Output the graph to a DOT file.
	void toDot(const std::string& filename) const;

protected:

	void buildCFG(const CFGNode& node, 
			std::map<CFGNode, Vertex>& nodesAdded,
			std::set<CFGNode>& nodesProcessed);

	void setEntryAndExit();

	void writeGraphNode(std::ostream& out, const Vertex& node) const;

	void writeGraphEdge(std::ostream& out, const Edge& edge) const;
};

typedef CFG<FullCFGType> FullCFG;
typedef CFG<FilteredCFGType> FilteredCFG;


template <class CFGType>
void CFG<CFGType>::toDot(const std::string& filename) const
{
    std::ofstream ofile(filename.c_str(), std::ios::out);
    write_graphviz(ofile, *this,
			boost::bind(&CFG<CFGType>::writeGraphNode, this, ::_1, ::_2),
			boost::bind(&CFG<CFGType>::writeGraphEdge, this, ::_1, ::_2));
}

template <class CFGType>
void CFG<CFGType>::build(SgFunctionDefinition* funcDef)
{
	ROSE_ASSERT(funcDef);

	this->clear();
	std::map<CFGNode, Vertex> nodesAdded;
	std::set<CFGNode> nodesProcessed;
	buildCFG(funcDef->cfgForBeginning(), nodesAdded, nodesProcessed);
	setEntryAndExit();

	ROSE_ASSERT(isSgFunctionDefinition((*this)[entry_].getNode()));
	ROSE_ASSERT(isSgFunctionDefinition((*this)[exit_].getNode()));
}

template <class CFGType>
void CFG<CFGType>::setEntryAndExit()
{
	typename boost::graph_traits<CFG<CFGType> >::vertex_iterator i, j;
	for (tie(i, j) = boost::vertices(*this); i != j; ++i)
	{
		CFGNode node = (*this)[*i];
		if (isSgFunctionDefinition(node.getNode()))
		{
			if (node.getIndex() == 0)
				entry_ = *i;
			else if (node.getIndex() == 3)
				exit_ = *i;
		}
	}
}

template <class CFGType>
void CFG<CFGType>::buildCFG(
		const CFGNode& node,
		std::map<CFGNode, Vertex>& nodesAdded,
		std::set<CFGNode>& nodesProcessed)
{
    ROSE_ASSERT(node.getNode());

    if (nodesProcessed.count(node) > 0)
        return;
	nodesProcessed.insert(node);

    typename std::map<CFGNode, Vertex>::iterator iter;
    bool inserted;
    Vertex from, to;

    // Add the source node.
    const CFGNode& src = node;
	ROSE_ASSERT(src.getNode());

    boost::tie(iter, inserted) = nodesAdded.insert(make_pair(src, Vertex()));

    if (inserted)
    {
        from = add_vertex(*this);
        (*this)[from] = src;
        iter->second = from;
    }
    else
        from = iter->second;

    std::vector<CFGEdge> outEdges = node.outEdges();
    foreach (const CFGEdge& edge, outEdges)
    {
        // For each out edge, add the target node.
        CFGNode tar = edge.target();
		ROSE_ASSERT(tar.getNode());

        boost::tie(iter, inserted) = nodesAdded.insert(make_pair(tar, Vertex()));

        if (inserted)
        {
            to = add_vertex(*this);
            (*this)[to] = tar;
            iter->second = to;
        }
        else
            to = iter->second;

        // Add the edge.
        (*this)[add_edge(from, to, *this).first] = edge;

        // Build the CFG recursively.
        buildCFG(tar, nodesAdded, nodesProcessed);
    }
}

template <class CFGType>
typename CFG<CFGType>::VertexToVertexMap CFG<CFGType>::buildDominatorTree() const
{
	VertexToVertexMap immediateDominators;
	boost::associative_property_map<VertexToVertexMap> domTreePredMap(immediateDominators);

	// Here we use the algorithm in boost::graph to build an map from each node to its immediate dominator.
	boost::lengauer_tarjan_dominator_tree(*this, entry_, domTreePredMap);
	return immediateDominators;
}

template <class CFGType>
typename CFG<CFGType>::VertexToVertexMap CFG<CFGType>::buildPostdominatorTree() const
{
	VertexToVertexMap immediatePostdominators;
	boost::associative_property_map<VertexToVertexMap> postdomTreePredMap(immediatePostdominators);

	// Here we use the algorithm in boost::graph to build an map from each node to its immediate dominator.
	boost::lengauer_tarjan_dominator_tree(boost::make_reverse_graph(*this), exit_, postdomTreePredMap);
	return immediatePostdominators;
}

template <class CFGType>
void CFG<CFGType>::writeGraphNode(std::ostream& out, const Vertex& node) const
{
	writeCFGNode(out, (*this)[node]);
}

template <class CFGType>
void CFG<CFGType>::writeGraphEdge(std::ostream& out, const Edge& edge) const
{
	writeCFGEdge(out, (*this)[edge]);
}

template <class CFGNodeType>
void writeCFGNode(std::ostream& out, const CFGNodeType& n)
{
	ROSE_ASSERT(n.getNode());

	std::string nodeColor = "black";
	if (isSgStatement(n.getNode()))
		nodeColor = "blue";
	else if (isSgExpression(n.getNode()))
		nodeColor = "green";
	else if (isSgInitializedName(n.getNode()))
		nodeColor = "red";

	out << "[label=\""  << escapeString(n.toString()) << "\", color=\"" << nodeColor <<
		"\", style=\"" << (n.isInteresting()? "solid" : "dotted") << "\"]";
}

template <class CFGEdgeType>
void writeCFGEdge(std::ostream& out, const CFGEdgeType& e)
{
	out << "[label=\"" << escapeString(e.toString()) <<
		"\", style=\"" << "solid" << "\"]";
}

} // End of namespace Backstroke


#undef foreach

#endif	/* BACKSTROKE_CFG_H */

