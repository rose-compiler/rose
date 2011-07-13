#ifndef ________CFG_H__________
#define	________CFG_H__________

#include <rose.h>
#include <boost/function.hpp>
#include <boost/graph/adjacency_list.hpp>

namespace Backstroke
{

typedef boost::function<bool(const VirtualCFG::CFGNode&)> CFGNodeFilter;


typedef VirtualCFG::FilteredCFGNode<CFGNodeFilter> CFGNode;
typedef VirtualCFG::FilteredCFGEdge<CFGNodeFilter> CFGEdge;

typedef boost::shared_ptr<CFGNode> CFGNodePtr;
typedef boost::shared_ptr<CFGEdge> CFGEdgePtr;

struct DefaultFilter
{
	bool operator()(const VirtualCFG::CFGNode& cfgNode) const
	{ return true; }
};

class ControlFlowGraph : public boost::adjacency_list<boost::vecS, boost::vecS, boost::bidirectionalS, 
		CFGNodePtr, CFGEdgePtr>
{
    typedef boost::graph_traits<ControlFlowGraph> GraphTraits;
public:
    typedef GraphTraits::vertex_descriptor Vertex;
	typedef GraphTraits::edge_descriptor   Edge;

	typedef std::map<Vertex, Vertex> VertexVertexMap;
    typedef std::set<Vertex> Vertices;
    
protected:

	//! The function definition of this CFG.
	SgFunctionDefinition* funcDef_;

	//! The entry node.
	Vertex entry_;

	//! The exit node.
	Vertex exit_;
    
    //! The CFG node filter.
    CFGNodeFilter filter_;

	//! A map from a CFG node to the corresponding vertex
	std::map<CFGNode, Vertex> nodesToVertices_;

    //! The dominator tree of this CFG.
    mutable VertexVertexMap dominatorTree_;

    //! The postdominator tree of this CFG.
    mutable VertexVertexMap postdominatorTree_;

public:

	//! The default constructor.
	ControlFlowGraph()
	:	funcDef_(NULL),
		entry_(GraphTraits::null_vertex()),
		exit_(GraphTraits::null_vertex()),
        filter_(DefaultFilter())
	{
	}

	//! The constructor building the CFG.
	explicit ControlFlowGraph(
            SgFunctionDefinition* funcDef, 
            const CFGNodeFilter& cfgNodeFilter = DefaultFilter())
	:	funcDef_(funcDef),
		entry_(GraphTraits::null_vertex()),
		exit_(GraphTraits::null_vertex()),
        filter_(cfgNodeFilter)
	{
		build(funcDef);
	}

	//! Build the actual CFG for the given function.
	void build(SgFunctionDefinition* funcDef);

	//! Get the function definition of this CFG.
	SgFunctionDefinition* getFunctionDefinition() const
	{ return funcDef_; }

	//! Get the entry node of the CFG
	Vertex getEntry() const { return entry_; }
    
	//! Get the exit node of the CFG
	Vertex getExit() const { return exit_; }
    
    //! Set the entry node of the CFG
    void setEntry(Vertex entry) { entry_ = entry; }
    
    //! Set the exit node of the CFG
    void setExit(Vertex exit) { exit_ = exit; }

	//! Build the dominator tree of this CFG.
	//! @returns A map from each node to its immediate dominator.
	const VertexVertexMap& getDominatorTree() const;

	//! Build the postdominator tree of this CFG.
	const VertexVertexMap& getPostdominatorTree() const;

	//! Build a reverse CFG.
	ControlFlowGraph makeReverseCopy() const;

	//! Output the graph to a DOT file.
	void toDot(const std::string& filename) const;

	//! Get all CFG nodes in this graph.
	std::vector<CFGNodePtr> getAllNodes() const;

	//! Get all CFG edges in this graph.
	std::vector<CFGEdgePtr> getAllEdges() const;

	//! Given a CFG node, returns the corresponding vertex in the graph.
	//! Returns Vertex::null_vertex() if the given node is not in the graph
	Vertex getVertexForNode(const CFGNode& node) const;

    //! Return if this CFG is reducible (if all loops are natural loops, the
    //! CFG is reducible).
    bool isReducible() const { return true; }

    //! Get all back edges in the CFG. A back edge is one whose target dominates its source.
    std::set<Edge> getAllBackEdges() const;

    //! Get all loop headers in this CFG. A natural loop only has one header.
    Vertices getAllLoopHeaders() const;
    
    //! Get all loops in this CFG. Each loop is represented by its header and all CFG nodes 
    //! belonging to it.
    std::map<Vertex, Vertices> getAllLoops() const;

protected:

	//! A internal funtion which builds the actual CFG (boost::graph).
	void buildCFG(const CFGNode& node,
			std::map<CFGNode, Vertex>& nodesAdded,
			std::set<CFGNode>& nodesProcessed);

	//! Find the entry and exit of this CFG and set the corresponding members.
	void setEntryAndExit();
    
    CFGNodePtr newCFGNode(const CFGNode& node)
    {
        CFGNodePtr newCFGNode(new CFGNode(node));
        //newCFGNode->setFilter(filter_);
        return newCFGNode;
    }
    
    CFGEdgePtr newCFGEdge(const CFGEdge& edge)
    {
        CFGEdgePtr newCFGEdge(new CFGEdge(edge));
        //newCFGEdge->setFilter(filter_);
        return newCFGEdge;
    }

	//! This function helps to write the DOT file for vertices.
	void writeGraphNode(std::ostream& out, const Vertex& node) const;

	//! This function helps to write the DOT file for edges.
	void writeGraphEdge(std::ostream& out, const Edge& edge) const;

	//! This class is used to copy vertices when calling copy_graph().
	struct VertexCopier
	{
		VertexCopier(const ControlFlowGraph& g1, ControlFlowGraph& g2)
		: cfg1(g1), cfg2(g2) {}

		void operator()(const Vertex& v1, Vertex& v2) const
		{ cfg2[v2] = cfg1[v1]; }
		
		const ControlFlowGraph& cfg1;
		ControlFlowGraph& cfg2;
	};

	//! This class is used to copy edges when calling copy_graph().
	struct EdgeCopier
	{
		EdgeCopier(const ControlFlowGraph& g1, ControlFlowGraph& g2)
		: cfg1(g1), cfg2(g2) {}

		void operator()(const Edge& e1, Edge& e2) const
		{ cfg2[e2] = cfg1[e1]; }

		const ControlFlowGraph& cfg1;
		ControlFlowGraph& cfg2;
	};
};


} // end of namespace


#endif	/* ________CFG_H__________ */

