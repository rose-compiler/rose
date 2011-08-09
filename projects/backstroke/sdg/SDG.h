#ifndef _______SDG_H_______
#define	_______SDG_H_______

#include "newPDG.h"

namespace SDG
{

typedef std::vector<SgInitializedName*> VarName;

struct SDGNode
{
    enum NodeType
    {
        Entry,
        ASTNode,
        FunctionCall,
        ActualIn,
        ActualOut,
        FormalIn,
        FormalOut
    };
    
    SDGNode(NodeType t) : type(t), astNode(NULL) {}
    
    NodeType type;
    
    CFGNodePtr cfgNode;
    
    SgNode* astNode;
    
    ////! The function definition for an entry node.
    //SgFunctionDefinition* funcDef;
    
};

struct SDGEdge
{
    //! The type of the SDG edge.
    enum EdgeType
    {
        ControlDependence,
        DataDependence,
        ParameterIn,
        ParameterOut,
        Call,
        Summary
    };
    
    SDGEdge(EdgeType t) : type(t) {}
    
    //! Indicate the type of this edge, whether control or data dependence.
	EdgeType type;
    
    //============================================================================//
    // Control dependence
    void setTrue()  { condition = VirtualCFG::eckTrue; }
    void setFalse() { condition = VirtualCFG::eckFalse; }
    
    //! The condition attached to edges in the CDG.
    VirtualCFG::EdgeConditionKind condition;

    //! If the condition is a case edge, this expression is the case value.
    SgExpression* caseLabel;

    
    //============================================================================//
    // Data dependence
    
	//! All variable names in data dependence of this edge.
	std::set<VarName> varNames;
};



class SystemDependenceGraph : public boost::adjacency_list<boost::vecS, boost::vecS, boost::bidirectionalS,
		SDGNode*, SDGEdge*>
{
public:
	typedef boost::graph_traits<SystemDependenceGraph>::vertex_descriptor Vertex;
	typedef boost::graph_traits<SystemDependenceGraph>::edge_descriptor   Edge;
    
protected:
    typedef ControlFlowGraph::Vertex CFGVertex;
	typedef ControlFlowGraph::Edge   CFGEdge;
    
protected:
    //! The CFG node filter which controls which AST nodes appear in CFG.
    CFGNodeFilter cfgNodefilter_;
    
    //! A table mapping each function to its CFG.
    std::map<SgFunctionDefinition*, ControlFlowGraph*> functionsToCFGs_;
    
    //! A table mapping each function to its entry in SDG.
    std::map<SgFunctionDefinition*, SDGNode*> functionsToEntries_;
    
public:
    SystemDependenceGraph(SgProject* project, CFGNodeFilter filter)
    : cfgNodefilter_(filter)
	{ build(project); }
    
    //! Build a SDG from a SgProject.
    void build(SgProject* project);
    
    void setCFGNodeFilter(CFGNodeFilter filter)
    { cfgNodefilter_ = filter; }
    
    
	//! Write the PDG to a dot file.
	void toDot(const std::string& filename) const;
    
protected:
    Vertex addVertex(SDGNode* sdgNode) 
    { 
        Vertex newVertex = boost::add_vertex(*this);
        (*this)[newVertex] = sdgNode;
        return newVertex;
    }
    
    Edge addEdge(Vertex src, Vertex tgt, SDGEdge* sdgEdge)
    {
        Edge newEdge = boost::add_edge(src, tgt, *this).first;
        (*this)[newEdge] = sdgEdge;
        return newEdge;
    }
    
    //! Check if a parameter of the given type in a function call is passed by reference or not.
    bool isParaPassedByRef(SgType* type) const;
    
    void addControlDependenceEdges(
        const std::map<CFGVertex, Vertex>& cfgVerticesToSdgVertices,
        const ControlFlowGraph& cfg, Vertex entry);
    
    void addDataDependenceEdges(
        const std::map<CFGVertex, Vertex>& cfgVerticesToSdgVertices,
        const ControlFlowGraph& cfg);
    
    //! This function helps to write the DOT file for vertices.
	void writeGraphNode(std::ostream& out, const Vertex& vertex) const;

	//! This function helps to write the DOT file for edges.
	void writeGraphEdge(std::ostream& out, const Edge& edge) const;
    
    //! This function helps to write the DOT file for the whole graph.
    void writeGraphProperty(std::ostream& out) const;
    
};

} // end of Backstroke

#endif	/* _______SDG_H_______ */

