#ifndef _______SDG_H_______
#define	_______SDG_H_______

#include "newPDG.h"
#include <boost/function.hpp>
#include <boost/unordered_map.hpp>

namespace SDG
{

typedef std::vector<SgInitializedName*> VarName;
typedef std::map<SgNode*, std::set<SgNode*> > DefUseChains;
    
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
    
    //CFGNodePtr cfgNode;
    
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
    
    enum ControlDependenceType
    {
        cdTrue,
        cdFalse,
        cdCase,
        cdDefault
    };
    
    SDGEdge(EdgeType t) : type(t) {}
    
    //! Indicate the type of this edge, whether control or data dependence.
	EdgeType type;
    
    //============================================================================//
    // Control dependence
    void setTrue()  { condition = cdTrue; }
    void setFalse() { condition = cdFalse; }
    void setCondition(VirtualCFG::EdgeConditionKind cond, SgExpression* expr = NULL);
    
    //! The condition attached to edges in the CDG.
    ControlDependenceType condition;

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
    
    typedef boost::function<void(SgProject*, DefUseChains&)> DefUseChainsGen;
    //typedef std::vector<DefUseChain> DefUseChains;
    
protected:
    
    //! The ROSE project object.
    SgProject* project_;
    
    //! The CFG node filter which controls which AST nodes appear in CFG.
    CFGNodeFilter cfgNodefilter_;
    
    //! A table mapping each function to its CFG.
    std::map<SgFunctionDeclaration*, ControlFlowGraph*> functionsToCFGs_;
    
    //! A table mapping each function to its entry in SDG.
    std::map<SgFunctionDeclaration*, Vertex> functionsToEntries_;
    
    boost::function<void(SgProject*, DefUseChains&)> defUseChainGenerator_;
    
public:
    SystemDependenceGraph(SgProject* project, CFGNodeFilter filter)
    : project_(project), cfgNodefilter_(filter)
	{}
    
    //! Build the SDG.
    void build();
    
    void setCFGNodeFilter(CFGNodeFilter filter)
    { cfgNodefilter_ = filter; }
    
    void setDefUseChainsGenerator(const DefUseChainsGen& defUseChainsGen)
    { defUseChainGenerator_ = defUseChainsGen; }
    
    
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
    
    //! Add a Control Dependence edge with True label.
    void addTrueCDEdge(Vertex src, Vertex tgt);
        
    void addControlDependenceEdges(
        const boost::unordered_map<CFGVertex, Vertex>& cfgVerticesToSdgVertices,
        const ControlFlowGraph& cfg, Vertex entry);
    
    void addDataDependenceEdges(
        const boost::unordered_map<CFGVertex, Vertex>& cfgVerticesToSdgVertices,
        const ControlFlowGraph& cfg,
        const std::map<SgNode*, Vertex>& formalOutPara);
    
    //! This function helps to write the DOT file for vertices.
	void writeGraphNode(std::ostream& out, const Vertex& vertex) const;

	//! This function helps to write the DOT file for edges.
	void writeGraphEdge(std::ostream& out, const Edge& edge) const;
    
    //! This function helps to write the DOT file for the whole graph.
    void writeGraphProperty(std::ostream& out) const;
    
};

} // end of Backstroke

#endif	/* _______SDG_H_______ */

