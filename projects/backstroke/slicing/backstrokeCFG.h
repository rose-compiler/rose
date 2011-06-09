#ifndef BACKSTROKE_CFG_H
#define	BACKSTROKE_CFG_H


#include <rose.h>
#include <boost/algorithm/string.hpp>
#include <boost/bind.hpp>
#include <boost/foreach.hpp>
#include <boost/graph/adjacency_list.hpp>
#include <boost/graph/dominator_tree.hpp>
#include <boost/graph/graphviz.hpp>
#include <boost/graph/reverse_graph.hpp>
#include <boost/graph/transpose_graph.hpp>
#include <boost/tuple/tuple.hpp>
#include <boost/vector_property_map.hpp>



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

	std::string label;

	if (SgFunctionDefinition* funcDef = isSgFunctionDefinition(node))
	{
		std::string funcName = funcDef->get_declaration()->get_name().str();
		if (cfgNode.getIndex() == 0)
			label = "Entry\\n" + funcName;
		else if (cfgNode.getIndex() == 3)
			label = "Exit\\n" + funcName;
	}
	
	if (!isSgScopeStatement(node) && !isSgCaseOptionStmt(node) && !isSgDefaultOptionStmt(node))
	{
		std::string content = node->unparseToString();
		boost::replace_all(content, "\"", "\\\"");
		boost::replace_all(content, "\\n", "\\\\n");
		label += content;
	}
    else
		label += "<" + node->class_name() + ">";
    
    if (label == "")
		label += "<" + node->class_name() + ">";
	
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
template <class CFGNodeFilter> class CFG;

struct FullCFGNodeFilter
{
	bool operator()(const VirtualCFG::CFGNode& cfgNode) const
	{ return true; }
};

struct InterestingCFGNodeFilter
{
	bool operator()(const VirtualCFG::CFGNode& cfgNode) const
	{ return cfgNode.isInteresting(); }
};

//! A full CFG without any filtered nodes.
typedef CFG<FullCFGNodeFilter> FullCFG;


//! A filtered CFG which only contains interesting nodes and edges.
typedef CFG<InterestingCFGNodeFilter> FilteredCFG;





/********************************************************************/
//	The concept required to be fulfilled by CFGNodeFilter is
//
//	struct CFGNodeFilter
//	{
//		bool operator()(const VirtualCFG::CFGNode& cfgNode) const;
//	};
//
/********************************************************************/

//! A class holding a Control Flow Graph.
	
template <class CFGNodeFilter>
class CFG : public boost::adjacency_list<boost::vecS, boost::vecS, boost::bidirectionalS, 
		boost::shared_ptr<VirtualCFG::FilteredCFGNode<CFGNodeFilter> >,
		boost::shared_ptr<VirtualCFG::FilteredCFGEdge<CFGNodeFilter> > >
{
	typedef typename boost::graph_traits<CFG<CFGNodeFilter> > GraphTraits;

public:
    typedef VirtualCFG::FilteredCFGNode<CFGNodeFilter> CFGNodeType;
	typedef VirtualCFG::FilteredCFGEdge<CFGNodeFilter> CFGEdgeType;

	typedef boost::shared_ptr<CFGNodeType> CFGNodePtr;
	typedef boost::shared_ptr<CFGEdgeType> CFGEdgePtr;

    typedef typename GraphTraits::vertex_descriptor Vertex;
	typedef typename GraphTraits::edge_descriptor Edge;

	typedef std::map<Vertex, Vertex> VertexVertexMap;
    typedef std::set<Vertex> Vertices;

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
    mutable VertexVertexMap dominatorTree_;

    //! The postdominator tree of this CFG.
    mutable VertexVertexMap postdominatorTree_;

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
	const VertexVertexMap& getDominatorTree() const;

	//! Build the postdominator tree of this CFG.
	const VertexVertexMap& getPostdominatorTree() const;

	//! Build a reverse CFG.
	CFG<CFGNodeFilter> makeReverseCopy() const;

	//! Output the graph to a DOT file.
	void toDot(const std::string& filename) const;

	//! Get all CFG nodes in this graph.
	std::vector<CFGNodePtr> getAllNodes() const;

	//! Get all CFG edges in this graph.
	std::vector<CFGEdgePtr> getAllEdges() const;

	//! Given a CFG node, returns the corresponding vertex in the graph.
	//! Returns Vertex::null_vertex() if the given node is not in the graph
	Vertex getVertexForNode(const CFGNodeType &node) const;

    //! Return if this CFG is reducible (if all loops are natural loops, the
    //! CFG is reducible).
    bool isReducible() const { return true; }

    //! Get all back edges in the CFG. A back edge is one whose target dominates its source.
    std::vector<Edge> getAllBackEdges() const;

    //! Get all loop headers in this CFG. A natural loop only has one header.
    std::vector<Vertex> getAllLoopHeaders() const;
    
    //! Get all loops in this CFG. Each loop is represented by its header and all CFG nodes 
    //! belonging to it.
    std::map<Vertex, Vertices> getAllLoops() const;

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
		VertexCopier(const CFG<CFGNodeFilter>& g1, CFG<CFGNodeFilter>& g2)
		: cfg1(g1), cfg2(g2) {}

		void operator()(const Vertex& v1, Vertex& v2) const
		{ cfg2[v2] = cfg1[v1]; }
		
		const CFG<CFGNodeFilter>& cfg1;
		CFG<CFGNodeFilter>& cfg2;
	};

	//! This class is used to copy edges when calling copy_graph().
	struct EdgeCopier
	{
		EdgeCopier(const CFG<CFGNodeFilter>& g1, CFG<CFGNodeFilter>& g2)
		: cfg1(g1), cfg2(g2) {}

		void operator()(const Edge& e1, Edge& e2) const
		{ cfg2[e2] = cfg1[e1]; }

		const CFG<CFGNodeFilter>& cfg1;
		CFG<CFGNodeFilter>& cfg2;
	};
};



template <class CFGNodeFilter>
void CFG<CFGNodeFilter>::toDot(const std::string& filename) const
{
	std::ofstream ofile(filename.c_str(), std::ios::out);
	boost::write_graphviz(ofile, *this,
			boost::bind(&CFG<CFGNodeFilter>::writeGraphNode, this, ::_1, ::_2),
			boost::bind(&CFG<CFGNodeFilter>::writeGraphEdge, this, ::_1, ::_2));
}

template <class CFGNodeFilter>
void CFG<CFGNodeFilter>::build(SgFunctionDefinition* funcDef)
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

template <class CFGNodeFilter>
void CFG<CFGNodeFilter>::setEntryAndExit()
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

template <class CFGNodeFilter>
void CFG<CFGNodeFilter>::buildCFG(
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

template <class CFGNodeFilter>
const typename CFG<CFGNodeFilter>::VertexVertexMap& CFG<CFGNodeFilter>::getDominatorTree() const
{
    if (!dominatorTree_.empty())
        return dominatorTree_;

	boost::associative_property_map<VertexVertexMap> domTreePredMap(dominatorTree_);

	// Here we use the algorithm in boost::graph to build a map from each node to its immediate dominator.
	boost::lengauer_tarjan_dominator_tree(*this, entry_, domTreePredMap);
	return dominatorTree_;
}

template <class CFGNodeFilter>
const typename CFG<CFGNodeFilter>::VertexVertexMap& CFG<CFGNodeFilter>::getPostdominatorTree() const
{
    if (!postdominatorTree_.empty())
        return postdominatorTree_;
    
	boost::associative_property_map<VertexVertexMap> postdomTreePredMap(postdominatorTree_);

	// Here we use the algorithm in boost::graph to build an map from each node to its immediate dominator.
	boost::lengauer_tarjan_dominator_tree(boost::make_reverse_graph(*this), exit_, postdomTreePredMap);
	return postdominatorTree_;
}

template <class CFGNodeFilter>
CFG<CFGNodeFilter> CFG<CFGNodeFilter>::makeReverseCopy() const
{
	CFG<CFGNodeFilter> reverseCFG;
	// The following function makes a reverse CFG copy.
	boost::transpose_graph(*this, reverseCFG, 
		boost::vertex_copy(VertexCopier(*this, reverseCFG)).
		edge_copy(EdgeCopier(*this, reverseCFG)));

	// Swap entry and exit.
	reverseCFG.entry_ = this->exit_;
	reverseCFG.exit_ = this->entry_;
	return reverseCFG;
}

template <class CFGNodeFilter>
std::vector<typename CFG<CFGNodeFilter>::CFGNodePtr>
CFG<CFGNodeFilter>::getAllNodes() const
{
	std::vector<CFGNodePtr> allNodes;
    foreach (Vertex v, boost::vertices(*this))
		allNodes.push_back((*this)[v]);
	return allNodes;
}

template <class CFGNodeFilter>
std::vector<typename CFG<CFGNodeFilter>::CFGEdgePtr>
CFG<CFGNodeFilter>::getAllEdges() const
{
	std::vector<CFGEdgePtr> allEdges;
    foreach (const Edge& e, boost::edges(*this))
		allEdges.push_back((*this)[e]);
	return allEdges;
}

template <class CFGNodeFilter>
typename CFG<CFGNodeFilter>::Vertex CFG<CFGNodeFilter>::getVertexForNode(const CFGNodeType &node) const
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

template <class CFGNodeFilter>
std::vector<typename CFG<CFGNodeFilter>::Edge> 
CFG<CFGNodeFilter>::getAllBackEdges() const
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

template <class CFGNodeFilter>
std::vector<typename CFG<CFGNodeFilter>::Vertex> 
CFG<CFGNodeFilter>::getAllLoopHeaders() const
{
    std::vector<Edge> backEdges = getAllBackEdges();
    std::vector<Vertex> headers;
    foreach (Edge e, backEdges)
        headers.push_back(boost::target(e, *this));
    return headers;
}

//! A DFS visitor used in depth first search.
template <typename VertexT> 
struct DFSVisitor : public boost::default_dfs_visitor
{
    DFSVisitor(std::set<VertexT>& vertices) : vertices_(vertices) {}
    template <typename Vertex, typename Graph>
    void discover_vertex(Vertex u, const Graph & g)
    { vertices_.insert(u); }
    std::set<VertexT>& vertices_;
};


template <class CFGNodeFilter>
std::map<typename CFG<CFGNodeFilter>::Vertex, typename CFG<CFGNodeFilter>::Vertices>
CFG<CFGNodeFilter>::getAllLoops() const
{
    std::vector<Edge> backEdges = getAllBackEdges();
    std::map<Vertex, Vertices> loops;
    
    // Build a reverse CFG.
    CFG<CFGNodeFilter> rvsCFG = makeReverseCopy();
    size_t verticesNum = boost::num_vertices(rvsCFG);
    
    // For each back edge, make the target (loop header) visited then
    // do a DFS from the source of this edge on the reverse CFG.
    foreach (const Edge& edge, backEdges)
    {
        Vertex root = boost::source(edge, rvsCFG);
        Vertex header = boost::target(edge, rvsCFG);
        Vertices vertices;
        
        // Create a DFS visitor in which we add all nodes to vertices set.
        DFSVisitor<Vertex> dfsVisitor(vertices);
        // Build a vector of colors and set all initial colors to white.
        std::vector<boost::default_color_type> colors(verticesNum, boost::white_color);
        // Set the color of the header to black.
        colors[header] = boost::black_color;

        // Do a DFS.
        boost::depth_first_visit(rvsCFG, root, dfsVisitor, &colors[0]);
        
        Vertices& verticesInLoop = loops[header];
        verticesInLoop.insert(vertices.begin(), vertices.end());
        verticesInLoop.insert(header);
    }
    return loops;
}

#undef foreach

} // End of namespace Backstroke


#endif	/* BACKSTROKE_CFG_H */

