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
#include <boost/algorithm/string.hpp>


namespace Backstroke
{

#define foreach BOOST_FOREACH


//! This function helps to write the DOT file for vertices.
template <class CFGNodeType>
void writeCFGNode(std::ostream& out, const CFGNodeType& cfgNode)
{
	SgNode* node = cfgNode.getNode();
	ROSE_ASSERT(node);

	std::string nodeColor = "black";
	if (isSgStatement(node))
		nodeColor = "blue";
	else if (isSgExpression(node))
		nodeColor = "green";
	else if (isSgInitializedName(node))
		nodeColor = "red";

	std::string label;// = escapeString(cfgNode.toString()) + "\\n";
	if (isSgFunctionDefinition(node))
	{
		if (cfgNode.getIndex() == 0)
			label = "Entry\\n";
		else if (cfgNode.getIndex() == 3)
			label = "Exit\\n";
	}
	
	if (!isSgScopeStatement(node))
	{
		std::string content = node->unparseToString();
		boost::replace_all(content, "\"", "\\\"");
		boost::replace_all(content, "\\n", "\\\\n");
		label += content;
	}
	
	out << "[label=\""  << label << "\", color=\"" << nodeColor <<
		"\", style=\"" << (cfgNode.isInteresting()? "solid" : "dotted") << "\"]";
}


//! This function helps to write the DOT file for edges.
template <class CFGEdgeType>
void writeCFGEdge(std::ostream& out, const CFGEdgeType& e)
{
	out << "[label=\"" << escapeString(e.toString()) <<
		"\", style=\"" << "solid" << "\"]";
}


// Predeclaration of class CFG.
template <class CFGNodeType, class CFGEdgeType> class CFG;

//! A full CFG without any filtered nodes.
typedef CFG<VirtualCFG::CFGNode,
			VirtualCFG::CFGEdge> FullCFG;


//! A filtered CFG which only contains interesting nodes and edges.
typedef CFG<VirtualCFG::InterestingNode,
			VirtualCFG::InterestingEdge> FilteredCFG;


struct CFGNodeFilter
{
	bool operator()(const VirtualCFG::CFGNode& cfgNode) const
	{
		if (!cfgNode.isInteresting())
			return false;

		SgNode* node = cfgNode.getNode();

		if (isSgValueExp(node))
			return false;
		//if (isSgExpression(node) && isSgExprStatement(node->get_parent()))
		if (isSgExprStatement(node))
			return false;
		if (isSgScopeStatement(node) && !isSgFunctionDefinition(node))
			return false;

		switch (node->variantT())
		{
			case V_SgVarRefExp:
			case V_SgInitializedName:
			case V_SgFunctionParameterList:
			case V_SgAssignInitializer:
			case V_SgFunctionRefExp:
			case V_SgPntrArrRefExp:
			case V_SgExprListExp:
			case V_SgCastExp:
			case V_SgForInitStatement:
			case V_SgCommaOpExp:
				return false;
			default:
				break;
		}
		
		return true;
	}
};

typedef CFG<VirtualCFG::FilteredCFGNode<CFGNodeFilter>,
			VirtualCFG::FilteredCFGEdge<CFGNodeFilter> > CFGForSSA;



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
//		bool operator<(const CFGNodeType&);
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
	
template <class CFGNodeT, class CFGEdgeT>
class CFG : public boost::adjacency_list<boost::vecS, boost::vecS, boost::bidirectionalS, 
		boost::shared_ptr<CFGNodeT>, boost::shared_ptr<CFGEdgeT> >
{
public:
	typedef CFGNodeT CFGNodeType;
	typedef CFGEdgeT CFGEdgeType;

	typedef boost::shared_ptr<CFGNodeT> CFGNodePtr;
	typedef boost::shared_ptr<CFGEdgeT> CFGEdgePtr;

	typedef typename boost::graph_traits<CFG<CFGNodeT, CFGEdgeT> > GraphTraits;
	typedef typename GraphTraits::vertex_descriptor Vertex;
	typedef typename GraphTraits::edge_descriptor Edge;

	typedef std::map<Vertex, Vertex> VertexVertexMap;

protected:

	//! The function definition of this CFG.
	SgFunctionDefinition* funcDef_;

	//! The entry node.
	Vertex entry_;

	//! The exit node.
	Vertex exit_;

public:

	//! The default constructor.
	CFG()
	:	funcDef_(NULL),
		entry_(GraphTraits::null_vertex()),
		exit_(GraphTraits::null_vertex())
	{
	}

	//! The constructor building the CFG.
	explicit CFG(SgFunctionDefinition* funcDef)
	:	funcDef_(funcDef),
		entry_(GraphTraits::null_vertex()),
		exit_(GraphTraits::null_vertex())
	{
		build(funcDef);
	}

	//! Build the actual CFG for the given function.
	void build(SgFunctionDefinition* funcDef);

	//! Get the function definition of this CFG.
	SgFunctionDefinition* getFunctionDefinition() const
	{ return funcDef_; }

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
	CFG<CFGNodeType, CFGEdgeType> makeReverseCopy() const;

	//! Output the graph to a DOT file.
	void toDot(const std::string& filename) const;

	//! Get all CFG nodes in this graph.
	std::vector<CFGNodePtr> getAllNodes() const;

	//! Get all CFG edges in this graph.
	std::vector<CFGEdgePtr> getAllEdges() const;

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
		writeCFGNode(out, *(*this)[node]);
		//VirtualCFG::printNode(out, (*this)[node]);
	}

	//! This function helps to write the DOT file for edges.
	void writeGraphEdge(std::ostream& out, const Edge& edge) const
	{
		writeCFGEdge(out, *(*this)[edge]);
		//VirtualCFG::printEdge(out, (*this)[edge], true);
	}

	//! This class is used to copy vertices when calling copy_graph().
	struct VertexCopier
	{
		VertexCopier(const CFG<CFGNodeType, CFGEdgeType>& g1, CFG<CFGNodeType, CFGEdgeType>& g2)
		: cfg1(g1), cfg2(g2) {}

		void operator()(const Vertex& v1, Vertex& v2) const
		{ cfg2[v2] = cfg1[v1]; }
		
		const CFG<CFGNodeType, CFGEdgeType>& cfg1;
		CFG<CFGNodeType, CFGEdgeType>& cfg2;
	};

	//! This class is used to copy edges when calling copy_graph().
	struct EdgeCopier
	{
		EdgeCopier(const CFG<CFGNodeType, CFGEdgeType>& g1, CFG<CFGNodeType, CFGEdgeType>& g2)
		: cfg1(g1), cfg2(g2) {}

		void operator()(const Edge& e1, Edge& e2) const
		{ cfg2[e2] = cfg1[e1]; }

		const CFG<CFGNodeType, CFGEdgeType>& cfg1;
		CFG<CFGNodeType, CFGEdgeType>& cfg2;
	};
};



template <class CFGNodeType, class CFGEdgeType>
void CFG<CFGNodeType, CFGEdgeType>::toDot(const std::string& filename) const
{
	std::ofstream ofile(filename.c_str(), std::ios::out);
	boost::write_graphviz(ofile, *this,
			boost::bind(&CFG<CFGNodeType, CFGEdgeType>::writeGraphNode, this, ::_1, ::_2),
			boost::bind(&CFG<CFGNodeType, CFGEdgeType>::writeGraphEdge, this, ::_1, ::_2));
}

template <class CFGNodeType, class CFGEdgeType>
void CFG<CFGNodeType, CFGEdgeType>::build(SgFunctionDefinition* funcDef)
{
	ROSE_ASSERT(funcDef);
	funcDef_ = funcDef;

	// The following two variables are used to record the nodes traversed.
	std::map<CFGNodeType, Vertex> nodesAdded;
	std::set<CFGNodeType> nodesProcessed;

	// Remove all nodes and edges first.
	this->clear();

	buildCFG(CFGNodeType(funcDef->cfgForBeginning()), nodesAdded, nodesProcessed);

	// Find the entry and exit of this CFG.
	setEntryAndExit();

	ROSE_ASSERT(isSgFunctionDefinition((*this)[entry_]->getNode()));
	ROSE_ASSERT(isSgFunctionDefinition((*this)[exit_]->getNode()));
}

template <class CFGNodeType, class CFGEdgeType>
void CFG<CFGNodeType, CFGEdgeType>::setEntryAndExit()
{
	typename boost::graph_traits<CFG<CFGNodeType, CFGEdgeType> >::vertex_iterator i, j;
	for (tie(i, j) = boost::vertices(*this); i != j; ++i)
	{
		CFGNodePtr node = (*this)[*i];
		if (isSgFunctionDefinition(node->getNode()))
		{
			if (node->getIndex() == 0)
				entry_ = *i;
			else if (node->getIndex() == 3)
				exit_ = *i;
		}
	}
}

template <class CFGNodeType, class CFGEdgeType>
void CFG<CFGNodeType, CFGEdgeType>::buildCFG(
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
		(*this)[from] = CFGNodePtr(new CFGNodeType(src));
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
			(*this)[to] = CFGNodePtr(new CFGNodeType(tar));
			iter->second = to;
		}
		else
			to = iter->second;

		// Add the edge.
		Edge edge = add_edge(from, to, *this).first;
		(*this)[edge] = CFGEdgePtr(new CFGEdgeType(cfgEdge));

		// Build the CFG recursively.
		buildCFG(tar, nodesAdded, nodesProcessed);
	}
}

template <class CFGNodeType, class CFGEdgeType>
typename CFG<CFGNodeType, CFGEdgeType>::VertexVertexMap CFG<CFGNodeType, CFGEdgeType>::buildDominatorTree() const
{
	VertexVertexMap immediateDominators;
	boost::associative_property_map<VertexVertexMap> domTreePredMap(immediateDominators);

	// Here we use the algorithm in boost::graph to build an map from each node to its immediate dominator.
	boost::lengauer_tarjan_dominator_tree(*this, entry_, domTreePredMap);
	return immediateDominators;
}

template <class CFGNodeType, class CFGEdgeType>
typename CFG<CFGNodeType, CFGEdgeType>::VertexVertexMap CFG<CFGNodeType, CFGEdgeType>::buildPostdominatorTree() const
{
	VertexVertexMap immediatePostdominators;
	boost::associative_property_map<VertexVertexMap> postdomTreePredMap(immediatePostdominators);

	// Here we use the algorithm in boost::graph to build an map from each node to its immediate dominator.
	boost::lengauer_tarjan_dominator_tree(boost::make_reverse_graph(*this), exit_, postdomTreePredMap);
	return immediatePostdominators;
}

template <class CFGNodeType, class CFGEdgeType>
CFG<CFGNodeType, CFGEdgeType> CFG<CFGNodeType, CFGEdgeType>::makeReverseCopy() const
{
	CFG<CFGNodeType, CFGEdgeType> reverseCFG;
	// The following function makes a reverse CFG copy.
	boost::transpose_graph(*this, reverseCFG, 
		boost::vertex_copy(VertexCopier(*this, reverseCFG)).
		edge_copy(EdgeCopier(*this, reverseCFG)));

	// Swap entry and exit.
	reverseCFG.entry_ = this->exit_;
	reverseCFG.exit_ = this->entry_;
	return reverseCFG;
}

template <class CFGNodeType, class CFGEdgeType>
std::vector<typename CFG<CFGNodeType, CFGEdgeType>::CFGNodePtr>
CFG<CFGNodeType, CFGEdgeType>::getAllNodes() const
{
	std::vector<CFGNodePtr> allNodes;
	typename boost::graph_traits<CFG<CFGNodeType, CFGEdgeType> >::vertex_iterator i, j;
	for (boost::tie(i, j) = boost::vertices(*this); i != j; ++i)
		allNodes.push_back((*this)[*i]);
	return allNodes;
}

template <class CFGNodeType, class CFGEdgeType>
std::vector<typename CFG<CFGNodeType, CFGEdgeType>::CFGEdgePtr>
CFG<CFGNodeType, CFGEdgeType>::getAllEdges() const
{
	std::vector<CFGEdgePtr> allEdges;
	typename boost::graph_traits<CFG<CFGNodeType, CFGEdgeType> >::edge_iterator i, j;
	for (boost::tie(i, j) = boost::edges(*this); i != j; ++i)
		allEdges.push_back((*this)[*i]);
	return allEdges;
}

#undef foreach

} // End of namespace Backstroke


#endif	/* BACKSTROKE_CFG_H */

