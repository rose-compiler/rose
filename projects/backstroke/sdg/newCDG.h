#ifndef ___________CDG_H__________
#define	___________CDG_H__________

#include "newCFG.h"

namespace SystemDependenceGraph
{


struct CDGEdge
{
    //! The corresponding CFG edge.
    ControlFlowGraph::Edge cfgEdge;
    
    //! The condition attached to edges in the CDG.
    VirtualCFG::EdgeConditionKind condition;

    //! If the condition is a case edge, this expression is the case value.
    SgExpression* caseLabel;
    
    std::string toString() const 
    {
        switch (condition) 
        {
        case eckTrue:
            return "T";
        case eckFalse:
            return "F";
        case eckCaseLabel:
            return "case" + caseLabel->unparseToString();
        case eckDefault:
            return "default";
        default: 
            break;
        }
        return "";
    }
};


class ControlDependenceGraph : public boost::adjacency_list<boost::vecS, boost::vecS, boost::bidirectionalS, 
		CFGNodePtr, CDGEdge>
{
public:    
    typedef boost::graph_traits<ControlDependenceGraph> GraphTraits;
	typedef GraphTraits::vertex_descriptor Vertex;
	typedef GraphTraits::edge_descriptor   Edge;


    struct ControlDependence 
    {
        ControlDependence(const CDGEdge& edge, SgNode* node)
        : cdEdge(edge), cdNode(node) {}

        //! Control dependence edge.
        CDGEdge cdEdge;

        ////! Control dependence graph node.
        //CDGNode cdNode;

        //! Control dependence node.
        SgNode* cdNode;
    };

    typedef std::vector<ControlDependence> ControlDependences;

	//! The default constructor.
	ControlDependenceGraph() {}

	//! The constructor building the CDG from a CFG.
	ControlDependenceGraph(const ControlFlowGraph& cfg)
	{ build(cfg); }

	//! Build the CDG from the given CFG.
	void build(const ControlFlowGraph& cfg);
    
    //! Given a CFG node, return all its control dependences.
    ControlDependences getControlDependences(CFGNodePtr cfgNode);
    
    //! Given a AST node, return all its control dependences.
    ControlDependences getControlDependences(SgNode* astNode);
    
    //! Given an AST node, return its corresponding CDG node. Note that it is possile
    //! that there are several CDG nodes containing the same AST node, in which case
    //! we just return one of them, since they have the same control dependence.
    Vertex getCDGVertex(SgNode* astNode);

	//! Write the CDG to a dot file.
	void toDot(const std::string& filename) const;

	//! This function helps to write the DOT file for edges.
	//! It's a static function which can be used by other classes (PDG for example).
	static void writeGraphEdge(std::ostream& out, const CDGEdge& edge);
	
protected:

	//! The data structure holding the dominance frontiers for all nodes in a CFG. Note that the control
	//! dependence information is all included. Each node is mapped to its all control dependent nodes.
	//! It is possible that a node is control dependent on another node in different conditions (switch case),
	//! so there is a vector of CFG edges for each control dependence.
	typedef std::map<
		ControlFlowGraph::Vertex,
		std::map<ControlFlowGraph::Vertex, 
				std::vector<ControlFlowGraph::Edge> > >
	DominanceFrontiersT;
	
	//! Check if the CFG contains any cycle without exit. If there is such a cycle, return false.
	bool checkCycle(const ControlFlowGraph& cfg);

	//! This function helps to write the DOT file for vertices.
	void writeGraphNode(std::ostream& out, const Vertex& node) const
	{
		writeCFGNode(out, *(*this)[node]);
	}

	//! This function helps to write the DOT file for edges.
	void writeGraphEdge(std::ostream& out, const Edge& edge) const
	{
		writeGraphEdge(out, (*this)[edge]);
	}

	//! A function help to find all successors for the given node in the CFG.
	void appendSuccessors(
		Vertex v,
		std::vector<Vertex>& vertices,
		const std::map<Vertex, std::set<Vertex> >& iSucc);

	//! Build dominance frontiers for all nodes in the given CFG.
	DominanceFrontiersT	buildDominanceFrontiers(
		const std::map<ControlFlowGraph::Vertex, ControlFlowGraph::Vertex>& iDom, const ControlFlowGraph& cfg);    
};


} // end of namespace

#endif	/* ___________CDG_H__________ */

