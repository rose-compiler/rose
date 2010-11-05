#ifndef BACKSTROKE_CFG_H
#define	BACKSTROKE_CFG_H


#include <rose.h>
#include <boost/graph/adjacency_list.hpp>

namespace Backstroke
{

using VirtualCFG::CFGNode;
using VirtualCFG::CFGEdge;


	//enum GraphNodeSgNodePropertyT { GraphNodeSgNodeProperty };
	//template <> struct property_kind<GraphNodeSgNodePropertyT> { typedef boost::vertex_property_tag type; }
	
class CFG
{
protected:

	//enum CFGEdgeAttributeT { CFGEdgeAttribute };
	//enum InterestingEdgeAttributeT { InterestingEdgeAttribute };

	typedef VirtualCFG::CFGNode CFGNode;
	typedef VirtualCFG::CFGEdge CFGEdge;
	typedef VirtualCFG::InterestingEdge InterestingEdge;

	//typedef boost::property<vertex_all_t, CFGNode> CFGNodeProperty;
	//typedef boost::property<CFGNodeAttributeT, CFGEdge*,
	//        boost::property<InterestingEdgeAttributeT, InterestingEdge*> > CFGEdgeProperty;

	typedef boost::adjacency_list<boost::vecS, boost::vecS, boost::directedS, CFGNode > Graph;

	typedef boost::graph_traits<Graph>::vertex_descriptor GraphNode;
	typedef boost::graph_traits<Graph>::edge_descriptor GraphEdge;

	//typedef boost::property_map<Graph, GraphNodeSgNodePropertyT>::type NodePropertyMap;
	//typedef boost::property_map<Graph, CFGEdgeAttributeT>::type CFGEdgeAttributeMap;
	//typedef boost::property_map<Graph, InterestingEdgeAttributeT>::type InterestingEdgeAttributeMap;

	//! The graph data structure holding the CFG.
	Graph graph_;

	//NodePropertyMap node_property_map_;


	////! A map from CFGNode in virtualCFG to node from staticCFG.
	//std::map<CFGNode, SgGraphNode*> all_nodes_;

	//! The entry node.
	GraphNode entry_;

	//! The exit node.
	GraphNode exit_;

	////! A flag shows whether this CFG is filtered or not.
	//bool is_filtered_;

public:

	CFG()
	{
	}

	//! The constructor building the CFG.

	/*! The valid nodes are SgProject, SgStatement, SgExpression and SgInitializedName. */
	CFG(SgFunctionDefinition* func_def)
	{
		//node_property_map_ = boost::get(GraphNodeSgNodePropertyT, graph_);
		//edge_property_map_ = boost::get(CFGEdgeAttribute, graph_);
		//interesting_edge_property_map_ = boost::get(InterestingEdgeAttribute, graph_);

		build(func_def);
	}

	//! Get the pointer pointing to the graph used by static CFG.

	const Graph& getGraph() const
	{
		return graph_;
	}

	virtual ~CFG()
	{
	}

	void build(SgFunctionDefinition* func_def);

	//! Get the entry node of the CFG

	const GraphNode& getEntry() const
	{
		return entry_;
	}

	//! Get the exit node of the CFG

	const GraphNode& getExit() const
	{
		return exit_;
	}
	
	//! Output the graph to a DOT file.
	void toDot(const std::string& filename);

protected:

	void buildCFG(const CFGNode& node, 
			std::map<CFGNode, GraphNode>& nodes_added,
			std::set<CFGNode>& nodes_processed);

	void writeGraphNode(std::ostream& out, const GraphNode& node);

	void writeGraphEdge(std::ostream& out, const GraphEdge& edge);

};

} // End of namespace Backstroke

#endif	/* BACKSTROKE_CFG_H */

