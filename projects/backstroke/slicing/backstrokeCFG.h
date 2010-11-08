#ifndef BACKSTROKE_CFG_H
#define	BACKSTROKE_CFG_H


#include <rose.h>
#include <boost/graph/adjacency_list.hpp>
#include <boost/bind.hpp>
#include <boost/foreach.hpp>
#include <boost/tuple/tuple.hpp>
#include <boost/graph/graphviz.hpp>
#include <boost/graph/dominator_tree.hpp>
#include <boost/graph/reverse_graph.hpp>
#include <boost/graph/transpose_graph.hpp>


namespace Backstroke
{

#define foreach BOOST_FOREACH

/********************************************************************/
//	The concept required to be fulfilled by CFGNodeType is
//
//	struct CFGNodeType
//	{
//		CFGNodeType();
//		std::vector<CFGEdgeType> outEdges() const;
//		SgNode* getNode() const;
//		int getIndex() const;
//		bool isInteresting() const;
//		std::string toString() const;
//	};
//
/********************************************************************/
//	The concept required to be fulfilled by CFGEdgeType is
//
//	struct CFGEdgeType
//	{
//		CFGEdgeType();
//		CFGNodeType target();
//		std::string toString() const;
//	};
//
/********************************************************************/

// TODO: We have to check if a CFG contains a cycle withou exit (infinite cycle), in which case
// we cannot build a CDG for it.

//! A class holding a Control Flow Graph.
	
template <class CFGNodeAndEdgeType>
class CFG : public boost::adjacency_list<boost::vecS, boost::vecS, boost::bidirectionalS, 
		typename CFGNodeAndEdgeType::CFGNodeType, typename CFGNodeAndEdgeType::CFGEdgeType>
{
public:
	typedef typename CFGNodeAndEdgeType::CFGNodeType CFGNodeType;
	typedef typename CFGNodeAndEdgeType::CFGEdgeType CFGEdgeType;

	typedef typename boost::graph_traits<CFG>::vertex_descriptor Vertex;
	typedef typename boost::graph_traits<CFG>::edge_descriptor Edge;

	typedef std::map<Vertex, Vertex> VertexVertexMap;

protected:
	//! The entry node.
	Vertex entry_;

	//! The exit node.
	Vertex exit_;

public:

	//! The default constructor.
	CFG()
	:	entry_(boost::graph_traits<CFG>::null_vertex()),
		exit_(boost::graph_traits<CFG>::null_vertex())
	{
	}

	//! The constructor building the CFG.
	CFG(SgFunctionDefinition* funcDef)
	:	entry_(boost::graph_traits<CFG>::null_vertex()),
		exit_(boost::graph_traits<CFG>::null_vertex())
	{
		build(funcDef);
	}

	virtual ~CFG() {}

	//! Build the actual CFG for the given function.
	void build(SgFunctionDefinition* funcDef);

	//! Get the entry node of the CFG
	const Vertex& getEntry() const
	{ return entry_; }

	//! Get the exit node of the CFG
	const Vertex& getExit() const
	{ return exit_; }

	//! Build the dominator tree of this CFG.
	VertexVertexMap buildDominatorTree() const;

	//! Build the postdominator tree of this CFG.
	VertexVertexMap buildPostdominatorTree() const;

	//! Build a reverse CFG.
	CFG<CFGNodeAndEdgeType> makeReverseCopy() const;

	//! Output the graph to a DOT file.
	void toDot(const std::string& filename) const;

protected:

	//! A internal funtion which builds the actual CFG (boost::graph).
	void buildCFG(const CFGNodeType& node,
			std::map<CFGNodeType, Vertex>& nodesAdded,
			std::set<CFGNodeType>& nodesProcessed);

	//! Find the entry and exit of this CFG and set the corresponding members.
	void setEntryAndExit();

	//! This function helps to write the DOT file for vertices.
	void writeGraphNode(std::ostream& out, const Vertex& node) const
	{
		writeCFGNode(out, (*this)[node]);
	}

	//! This function helps to write the DOT file for edges.
	void writeGraphEdge(std::ostream& out, const Edge& edge) const
	{
		writeCFGEdge(out, (*this)[edge]);
	}

	//! This class is used to copy vertices when calling copy_graph().
	struct VertexCopier
	{
		VertexCopier(const CFG<CFGNodeAndEdgeType>& g1, CFG<CFGNodeAndEdgeType>& g2)
		: cfg1(g1), cfg2(g2) {}

		void operator()(const Vertex& v1, Vertex& v2) const
		{ cfg2[v2] = cfg1[v1]; }
		
		const CFG<CFGNodeAndEdgeType>& cfg1;
		CFG<CFGNodeAndEdgeType>& cfg2;
	};

	//! This class is used to copy edges when calling copy_graph().
	struct EdgeCopier
	{
		EdgeCopier(const CFG<CFGNodeAndEdgeType>& g1, CFG<CFGNodeAndEdgeType>& g2)
		: cfg1(g1), cfg2(g2) {}

		void operator()(const Edge& e1, Edge& e2) const
		{ cfg2[e2] = cfg1[e1]; }

		const CFG<CFGNodeAndEdgeType>& cfg1;
		CFG<CFGNodeAndEdgeType>& cfg2;
	};
};


struct FullCFGType
{
	typedef VirtualCFG::CFGNode CFGNodeType;
	typedef VirtualCFG::CFGEdge CFGEdgeType;
};

//! A full CFG without any filtered nodes.
typedef CFG<FullCFGType> FullCFG;

struct FilteredCFGType
{
	typedef VirtualCFG::InterestingNode CFGNodeType;
	typedef VirtualCFG::InterestingEdge CFGEdgeType;
};

//! A filtered CFG which only contains interesting nodes and edges.
typedef CFG<FilteredCFGType> FilteredCFG;


template <class CFGNodeAndEdgeType>
void CFG<CFGNodeAndEdgeType>::toDot(const std::string& filename) const
{
	std::ofstream ofile(filename.c_str(), std::ios::out);
	boost::write_graphviz(ofile, *this,
			boost::bind(&CFG<CFGNodeAndEdgeType>::writeGraphNode, this, ::_1, ::_2),
			boost::bind(&CFG<CFGNodeAndEdgeType>::writeGraphEdge, this, ::_1, ::_2));
}

template <class CFGNodeAndEdgeType>
void CFG<CFGNodeAndEdgeType>::build(SgFunctionDefinition* funcDef)
{
	ROSE_ASSERT(funcDef);

	this->clear();

	// The following two variables are used to record the nodes traversed.
	std::map<CFGNodeType, Vertex> nodesAdded;
	std::set<CFGNodeType> nodesProcessed;
	
	buildCFG(funcDef->cfgForBeginning(), nodesAdded, nodesProcessed);

	// Find the entry and exit of this CFG.
	setEntryAndExit();

	ROSE_ASSERT(isSgFunctionDefinition((*this)[entry_].getNode()));
	ROSE_ASSERT(isSgFunctionDefinition((*this)[exit_].getNode()));
}

template <class CFGNodeAndEdgeType>
void CFG<CFGNodeAndEdgeType>::setEntryAndExit()
{
	typename boost::graph_traits<CFG<CFGNodeAndEdgeType> >::vertex_iterator i, j;
	for (tie(i, j) = boost::vertices(*this); i != j; ++i)
	{
		CFGNodeType node = (*this)[*i];
		if (isSgFunctionDefinition(node.getNode()))
		{
			if (node.getIndex() == 0)
				entry_ = *i;
			else if (node.getIndex() == 3)
				exit_ = *i;
		}
	}
}

template <class CFGNodeAndEdgeType>
void CFG<CFGNodeAndEdgeType>::buildCFG(
		const CFGNodeType& node,
		std::map<CFGNodeType, Vertex>& nodesAdded,
		std::set<CFGNodeType>& nodesProcessed)
{
	ROSE_ASSERT(node.getNode());

	if (nodesProcessed.count(node) > 0)
		return;
	nodesProcessed.insert(node);

	typename std::map<CFGNodeType, Vertex>::iterator iter;
	bool inserted;
	Vertex from, to;

	// Add the source node.
	const CFGNodeType& src = node;
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

	std::vector<CFGEdgeType> outEdges = node.outEdges();

	foreach(const CFGEdgeType& cfgEdge, outEdges)
	{
		// For each out edge, add the target node.
		CFGNodeType tar = cfgEdge.target();
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
		Edge edge = add_edge(from, to, *this).first;
		(*this)[edge] = cfgEdge;

		// Build the CFG recursively.
		buildCFG(tar, nodesAdded, nodesProcessed);
	}
}

template <class CFGNodeAndEdgeType>
typename CFG<CFGNodeAndEdgeType>::VertexVertexMap CFG<CFGNodeAndEdgeType>::buildDominatorTree() const
{
	VertexVertexMap immediateDominators;
	boost::associative_property_map<VertexVertexMap> domTreePredMap(immediateDominators);

	// Here we use the algorithm in boost::graph to build an map from each node to its immediate dominator.
	boost::lengauer_tarjan_dominator_tree(*this, entry_, domTreePredMap);
	return immediateDominators;
}

template <class CFGNodeAndEdgeType>
typename CFG<CFGNodeAndEdgeType>::VertexVertexMap CFG<CFGNodeAndEdgeType>::buildPostdominatorTree() const
{
	VertexVertexMap immediatePostdominators;
	boost::associative_property_map<VertexVertexMap> postdomTreePredMap(immediatePostdominators);

	// Here we use the algorithm in boost::graph to build an map from each node to its immediate dominator.
	boost::lengauer_tarjan_dominator_tree(boost::make_reverse_graph(*this), exit_, postdomTreePredMap);
	return immediatePostdominators;
}

template <class CFGNodeAndEdgeType>
CFG<CFGNodeAndEdgeType> CFG<CFGNodeAndEdgeType>::makeReverseCopy() const
{
	CFG<CFGNodeAndEdgeType> reverseCFG;
	// The following function makes a reverse CFG copy.
	boost::transpose_graph(*this, reverseCFG, 
		boost::vertex_copy(VertexCopier(*this, reverseCFG)).
		edge_copy(EdgeCopier(*this, reverseCFG)));

	// Swap entry and exit.
	reverseCFG.entry_ = this->exit_;
	reverseCFG.exit_ = this->entry_;
	return reverseCFG;
}

//! This function helps to write the DOT file for vertices.
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


//! This function helps to write the DOT file for edges.
template <class CFGEdgeType>
void writeCFGEdge(std::ostream& out, const CFGEdgeType& e)
{
	out << "[label=\"" << escapeString(e.toString()) <<
		"\", style=\"" << "solid" << "\"]";
}

#undef foreach

} // End of namespace Backstroke


#endif	/* BACKSTROKE_CFG_H */

