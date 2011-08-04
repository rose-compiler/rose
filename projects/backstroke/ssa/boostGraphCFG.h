#pragma once


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


#define foreach BOOST_FOREACH




//! A class holding a Control Flow Graph.
template <class CFGNodeT, class CFGEdgeT>
class CFG : public boost::adjacency_list<boost::vecS, boost::vecS, boost::bidirectionalS, 
		boost::shared_ptr<CFGNodeT>, boost::shared_ptr<CFGEdgeT> >
{
public:
	typedef typename boost::graph_traits<CFG<CFGNodeT, CFGEdgeT> > GraphTraits;


    typedef CFGNodeT CFGNodeType;
	typedef CFGEdgeT CFGEdgeType;

	typedef boost::shared_ptr<CFGNodeType> CFGNodePtr;
	typedef boost::shared_ptr<CFGEdgeType> CFGEdgePtr;

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

	//! A map from a CFG node to the corresponding vertex
	std::map<CFGNodeType, Vertex> nodesToVertices_;

    //! The dominator tree of this CFG.
    VertexVertexMap dominatorTree_;

    //! The postdominator tree of this CFG.
    VertexVertexMap postdominatorTree_;

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
	//! @returns A map from each node to its immediate dominator.
	const VertexVertexMap& getDominatorTree();

	//! Build the postdominator tree of this CFG.
	const VertexVertexMap& getPostdominatorTree();

	//! Build a reverse CFG.
	CFG<CFGNodeT, CFGEdgeT> makeReverseCopy() const;

	//! Get all CFG nodes in this graph.
	std::vector<CFGNodePtr> getAllNodes() const;

	//! Get all CFG edges in this graph.
	std::vector<CFGEdgePtr> getAllEdges() const;

	//! Given a CFG node, returns the corresponding vertex in the graph.
	//! Returns Vertex::null_vertex() if the given node is not in the graph
	Vertex getVertexForNode(const CFGNodeType &node) const;

    //! Get all back edges in the CFG. A back edge is one whose target dominates its source.
    std::vector<Edge> getAllBackEdges();

    //! Get all loop headers in this CFG. A natural loop only has one header.
    std::vector<Vertex> getAllLoopHeaders();

protected:

	//! A internal funtion which builds the actual CFG (boost::graph).
	void buildCFG(const CFGNodeType& node,
			std::map<CFGNodeType, Vertex>& nodesAdded,
			std::set<CFGNodeType>& nodesProcessed);

	//! Find the entry and exit of this CFG and set the corresponding members.
	void setEntryAndExit();

	//! This class is used to copy vertices when calling copy_graph().
	struct VertexCopier
	{
		VertexCopier(const CFG<CFGNodeT, CFGEdgeT>& g1, CFG<CFGNodeT, CFGEdgeT>& g2)
		: cfg1(g1), cfg2(g2) {}

		void operator()(const Vertex& v1, Vertex& v2) const
		{ cfg2[v2] = cfg1[v1]; }
		
		const CFG<CFGNodeT, CFGEdgeT>& cfg1;
		CFG<CFGNodeT, CFGEdgeT>& cfg2;
	};

	//! This class is used to copy edges when calling copy_graph().
	struct EdgeCopier
	{
		EdgeCopier(const CFG<CFGNodeT, CFGEdgeT>& g1, CFG<CFGNodeT, CFGEdgeT>& g2)
		: cfg1(g1), cfg2(g2) {}

		void operator()(const Edge& e1, Edge& e2) const
		{ cfg2[e2] = cfg1[e1]; }

		const CFG<CFGNodeT, CFGEdgeT>& cfg1;
		CFG<CFGNodeT, CFGEdgeT>& cfg2;
	};
};





template <class CFGNodeT, class CFGEdgeT>
void CFG<CFGNodeT, CFGEdgeT>::build(SgFunctionDefinition* funcDef)
{
	ROSE_ASSERT(funcDef);
	funcDef_ = funcDef;

	// The following two variables are used to record the nodes traversed.
	nodesToVertices_.clear();
	std::set<CFGNodeType> nodesProcessed;

	// Remove all nodes and edges first.
	this->clear();
	entry_ = GraphTraits::null_vertex();
	exit_ = GraphTraits::null_vertex();

	buildCFG(CFGNodeType(funcDef->cfgForBeginning()), nodesToVertices_, nodesProcessed);

	// Find the entry and exit of this CFG.
	setEntryAndExit();

	ROSE_ASSERT(isSgFunctionDefinition((*this)[entry_]->getNode()));
	ROSE_ASSERT(isSgFunctionDefinition((*this)[exit_]->getNode()));
}

template <class CFGNodeT, class CFGEdgeT>
void CFG<CFGNodeT, CFGEdgeT>::setEntryAndExit()
{
    foreach (Vertex v, boost::vertices(*this))
	{
		CFGNodePtr node = (*this)[v];
		if (isSgFunctionDefinition(node->getNode()))
		{
			if (node->getIndex() == 0)
				entry_ = v;
			else if (node->getIndex() == 3)
				exit_ = v;
		}
	}

	//In graphs with an infinite loop, we might never get to the end vertex
	//In those cases, we need to add it explicitly
	if (exit_ == GraphTraits::null_vertex())
	{
		std::cerr << "This function may contain an infinite loop "
				"inside so that its CFG cannot be built" << std::endl;
		exit_ = add_vertex(*this);
		(*this)[exit_] = CFGNodePtr(new CFGNodeType(funcDef_->cfgForEnd()));
	}

	ROSE_ASSERT(entry_ != GraphTraits::null_vertex());
	ROSE_ASSERT(exit_ != GraphTraits::null_vertex());
}

template <class CFGNodeT, class CFGEdgeT>
void CFG<CFGNodeT, CFGEdgeT>::buildCFG(
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

	boost::tie(iter, inserted) = nodesAdded.insert(std::make_pair(src, Vertex()));

	if (inserted)
	{
		from = add_vertex(*this);
		(*this)[from] = CFGNodePtr(new CFGNodeType(src));
		iter->second = from;
	}
	else
	{
		from = iter->second;
	}

	std::vector<CFGEdgeType> outEdges = node.outEdges();

	foreach(const CFGEdgeType& cfgEdge, outEdges)
	{
		// For each out edge, add the target node.
		CFGNodeType tar = cfgEdge.target();
		ROSE_ASSERT(tar.getNode());

		boost::tie(iter, inserted) = nodesAdded.insert(std::make_pair(tar, Vertex()));

		if (inserted)
		{
			to = add_vertex(*this);
			(*this)[to] = CFGNodePtr(new CFGNodeType(tar));
			iter->second = to;
		}
		else
		{
			to = iter->second;
		}

		// Add the edge.
		Edge edge = add_edge(from, to, *this).first;
		(*this)[edge] = CFGEdgePtr(new CFGEdgeType(cfgEdge));

		// Build the CFG recursively.
		buildCFG(tar, nodesAdded, nodesProcessed);
	}
}

template <class CFGNodeT, class CFGEdgeT>
const typename CFG<CFGNodeT, CFGEdgeT>::VertexVertexMap& CFG<CFGNodeT, CFGEdgeT>::getDominatorTree()
{
    if (!dominatorTree_.empty())
        return dominatorTree_;

	boost::associative_property_map<VertexVertexMap> domTreePredMap(dominatorTree_);

	// Here we use the algorithm in boost::graph to build a map from each node to its immediate dominator.
	boost::lengauer_tarjan_dominator_tree(*this, entry_, domTreePredMap);
	return dominatorTree_;
}

template <class CFGNodeT, class CFGEdgeT>
const typename CFG<CFGNodeT, CFGEdgeT>::VertexVertexMap& CFG<CFGNodeT, CFGEdgeT>::getPostdominatorTree()
{
    if (!postdominatorTree_.empty())
        return postdominatorTree_;
    
	boost::associative_property_map<VertexVertexMap> postdomTreePredMap(postdominatorTree_);

	// Here we use the algorithm in boost::graph to build an map from each node to its immediate dominator.
	boost::lengauer_tarjan_dominator_tree(boost::make_reverse_graph(*this), exit_, postdomTreePredMap);
	return postdominatorTree_;
}

template <class CFGNodeT, class CFGEdgeT>
CFG<CFGNodeT, CFGEdgeT> CFG<CFGNodeT, CFGEdgeT>::makeReverseCopy() const
{
	CFG<CFGNodeT, CFGEdgeT> reverseCFG;
	// The following function makes a reverse CFG copy.
	boost::transpose_graph(*this, reverseCFG, 
		boost::vertex_copy(VertexCopier(*this, reverseCFG)).
		edge_copy(EdgeCopier(*this, reverseCFG)));

	// Swap entry and exit.
	reverseCFG.entry_ = this->exit_;
	reverseCFG.exit_ = this->entry_;
	return reverseCFG;
}

template <class CFGNodeT, class CFGEdgeT>
std::vector<typename CFG<CFGNodeT, CFGEdgeT>::CFGNodePtr>
CFG<CFGNodeT, CFGEdgeT>::getAllNodes() const
{
	std::vector<CFGNodePtr> allNodes;
    foreach (Vertex v, boost::vertices(*this))
		allNodes.push_back((*this)[v]);
	return allNodes;
}

template <class CFGNodeT, class CFGEdgeT>
std::vector<typename CFG<CFGNodeT, CFGEdgeT>::CFGEdgePtr>
CFG<CFGNodeT, CFGEdgeT>::getAllEdges() const
{
	std::vector<CFGEdgePtr> allEdges;
    foreach (const Edge& e, boost::edges(*this))
		allEdges.push_back((*this)[e]);
	return allEdges;
}

template <class CFGNodeT, class CFGEdgeT>
typename CFG<CFGNodeT, CFGEdgeT>::Vertex CFG<CFGNodeT, CFGEdgeT>::getVertexForNode(const CFGNodeType &node) const
{
	typename std::map<CFGNodeType, Vertex>::const_iterator vertexIter = nodesToVertices_.find(node);
	if (vertexIter == nodesToVertices_.end())
		return GraphTraits::null_vertex();
	else
	{
		ROSE_ASSERT(*(*this)[vertexIter->second] == node);
		return vertexIter->second;
	}
}

template <class CFGNodeT, class CFGEdgeT>
std::vector<typename CFG<CFGNodeT, CFGEdgeT>::Edge> CFG<CFGNodeT, CFGEdgeT>::getAllBackEdges()
{
    std::vector<Edge> backEdges;

    // If the dominator tree is not built yet, build it now.
    getDominatorTree();

    foreach (const Edge& e, boost::edges(*this))
    {
        Vertex src = boost::source(e, *this);
        Vertex tar = boost::target(e, *this);

        //Vertex v = *(dominatorTree.find(src));
        typename VertexVertexMap::const_iterator iter = dominatorTree_.find(src);
        while (iter != dominatorTree_.end())
        {
            if (iter->second == tar)
            {
                backEdges.push_back(e);
                break; // break the while loop
            }
            iter = dominatorTree_.find(iter->second);
        }
    }

    return backEdges;
}

template <class CFGNodeT, class CFGEdgeT>
std::vector<typename CFG<CFGNodeT, CFGEdgeT>::Vertex> CFG<CFGNodeT, CFGEdgeT>::getAllLoopHeaders()
{
    std::vector<Edge> backEdges = getAllBackEdges();
    std::vector<Vertex> headers;
    foreach (Edge e, backEdges)
        headers.push_back(boost::target(e, *this));
    return headers;
}

#undef foreach


