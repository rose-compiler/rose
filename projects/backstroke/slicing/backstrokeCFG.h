#ifndef BACKSTROKE_CFG_H
#define	BACKSTROKE_CFG_H


#include <rose.h>
#include <boost/graph/adjacency_list.hpp>

namespace Backstroke
{


typedef VirtualCFG::CFGNode CFGNode;
typedef VirtualCFG::CFGEdge CFGEdge;

class CFG : public boost::adjacency_list<boost::vecS, boost::vecS, boost::bidirectionalS, CFGNode>
{
public:
	//typedef VirtualCFG::CFGEdge CFGEdge;
	//typedef VirtualCFG::InterestingEdge InterestingEdge;

	//typedef boost::property<vertex_all_t, CFGNode> CFGNodeProperty;
	//typedef boost::property<CFGNodeAttributeT, CFGEdge*,
	//        boost::property<InterestingEdgeAttributeT, InterestingEdge*> > CFGEdgeProperty;

	//typedef boost::adjacency_list<boost::vecS, boost::vecS, boost::directedS, CFGNode > Graph;

	typedef boost::graph_traits<CFG>::vertex_descriptor Vertex;
	typedef boost::graph_traits<CFG>::edge_descriptor Edge;

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

void writeCFGNode(std::ostream& out, const CFGNode& n);

void writeCFGEdge(std::ostream& out, const CFGEdge& e);

} // End of namespace Backstroke

#endif	/* BACKSTROKE_CFG_H */

