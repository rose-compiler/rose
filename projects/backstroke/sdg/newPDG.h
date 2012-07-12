#ifndef ___________PDG_H__________
#define	___________PDG_H__________

#include "newCDG.h"
#include "newDDG.h"

namespace SDG
{


typedef std::vector<SgInitializedName*> VarName;
        
struct PDGNode
{
    bool isEntry;
    CFGNodePtr cfgNode;
};


//! Define the edge type of PDG.

//! Note that in this structure, if its type is "ControlDependence", only the member
//! "key" is used, else only the member "varNames" is used.
struct PDGEdge
{
	//typedef std::vector<SgInitializedName*> VarName;
	
	enum EdgeType
	{
		ControlDependence,
		DataDependence
	};

	//! Indicate the type of this edge, whether control or data dependence.
	EdgeType type;
    
    /**************************************************************************/    
    // Control dependence
    
    //! The condition attached to edges in the CDG.
    VirtualCFG::EdgeConditionKind condition;

    //! If the condition is a case edge, this expression is the case value.
    SgExpression* caseLabel;

    /**************************************************************************/  
    
    
    
    /**************************************************************************/    
    // Data dependence
    
	//! All variable names in data dependence of this edge.
	std::set<VarName> varNames;
    
    /**************************************************************************/    
    
	//! A control dependence edge.
	CDGEdge cdEdge;

	//! A data dependence edge.
	DDGEdge ddEdge;

	////! All variable names in data dependence of this edge.
	//std::set<VarName> varNames;
};

class ProgramDependenceGraph : public boost::adjacency_list<boost::vecS, boost::vecS, boost::bidirectionalS,
		CFGNodePtr, PDGEdge >
{
public:
	typedef boost::graph_traits<ProgramDependenceGraph>::vertex_descriptor Vertex;
	typedef boost::graph_traits<ProgramDependenceGraph>::edge_descriptor   Edge;

protected:
    typedef ControlFlowGraph::Vertex CFGVertex;
	typedef ControlFlowGraph::Edge   CFGEdge;
    
    
    //! The entry of the PDG. Vertices which postdominate the entry of the CFG are
    //! control dependent on this vectex with label T.
    Vertex entry_;
    
public:
	//! The default constructor.
	ProgramDependenceGraph() {}

	//! The constructor building the PDG from a CFG.
	ProgramDependenceGraph(const ControlFlowGraph& cfg)
	{
		build(cfg);
	}

	//! The constructor building the PDG from a function definition.
	ProgramDependenceGraph(SgFunctionDefinition* funcDef)
	{
		ControlFlowGraph cfg(funcDef);
		build(cfg);
	}

	//! Build the PDG from the given CFG.
	void build(const ControlFlowGraph& cfg);
    
    //! Build the PDG from the given CFG.
	void build2(const ControlFlowGraph& cfg);

	//! Write the PDG to a dot file.
	void toDot(const std::string& filename) const;
    
    Vertex getEntry() const { return entry_; }

protected:

    void addControlDependenceEdges(
        const std::map<CFGVertex, Vertex>& cfgVerticesToPdgVertices,
        const ControlFlowGraph& cfg);
    
    void addDataDependenceEdges(
        const std::map<CFGVertex, Vertex>& cfgVerticesToPdgVertices,
        const ControlFlowGraph& cfg);
    
	//! This function helps to write the DOT file for vertices.
	void writeGraphNode(std::ostream& out, const Vertex& node) const
	{
        if (node == entry_)
        {
            out << "[label=\"ENTRY\"]";
        }
        else
            writeCFGNode(out, *(*this)[node]);
	}

	//! This function helps to write the DOT file for edges.
	void writeGraphEdge(std::ostream& out, const Edge& edge) const;
};



} // end of namespace SystemDependenceGraph



#endif	/* ___________PDG_H__________ */

