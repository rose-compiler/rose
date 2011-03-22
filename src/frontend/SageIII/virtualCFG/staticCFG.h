#ifndef STATIC_CFG_H 
#define STATIC_CFG_H 

#include <sage3basic.h>
#include "AstAttributeMechanism.h"
#include "virtualCFG.h"
#include <map>
#include <set>
#include <string>


class SgIncidenceDirectedGraph;
class SgGraphNode;
class SgDirectedGraphEdge;


namespace StaticCFG 
{

using VirtualCFG::CFGNode;
using VirtualCFG::CFGEdge;


class CFG
{
protected:
    //! The graph data structure holding the CFG.
    SgIncidenceDirectedGraph* graph_;

    //! A map from CFGNode in virtualCFG to node from staticCFG.
    std::map<CFGNode, SgGraphNode*> all_nodes_;

    //! The start node to begin CFG build.
    SgNode* start_;

    //! The entry node.
    SgGraphNode* entry_;

    //! The exit node.
    SgGraphNode* exit_;

    //! A flag shows whether this CFG is filtered or not.
    bool is_filtered_;

public:
    CFG() : graph_(NULL), start_(NULL), entry_(NULL), exit_(NULL) {}

    //! The constructor building the CFG.
    /*! The valid nodes are SgProject, SgStatement, SgExpression and SgInitializedName. */
    CFG(SgNode* node, bool is_filtered = false)
        : graph_(NULL), start_(node), entry_(NULL), exit_(NULL), is_filtered_(is_filtered)
    { buildCFG(); }

    //! Get the pointer pointing to the graph used by static CFG.
    SgIncidenceDirectedGraph* getGraph() const
    { return graph_; }

    virtual ~CFG()
    { clearNodesAndEdges(); }

    //! Set the start node for graph building. 
    /*! The valid nodes are SgProject, SgStatement, SgExpression and SgInitializedName. */
    void setStart(SgNode* node) { start_ = node; }

    //! Get the entry node of the CFG
    SgGraphNode* getEntry() const
    { return entry_; }

    //! Get the exit node of the CFG
    SgGraphNode* getExit() const
    { return exit_; }

    bool isFilteredCFG() const { return is_filtered_; }
    void setFiltered(bool flag) { is_filtered_ = flag; }


    //! Build CFG according to the 'is_filtered_' flag.
    virtual void buildCFG()
    {
        if (is_filtered_) buildFilteredCFG();
        else buildFullCFG();
    }

    //! Build CFG for debugging.
    virtual void buildFullCFG();

    //! Build filtered CFG which only contains interesting nodes.
    virtual void buildFilteredCFG();
    

#if 0
    std::vector<SgDirectedGraphEdge*> getOutEdges(SgNode* node, int index);
    std::vector<SgDirectedGraphEdge*> getInEdges(SgNode* node, int index);
#endif

    // The following four functions are for getting in/out edges of a given node.
    std::vector<SgDirectedGraphEdge*> getOutEdges(SgGraphNode* node);
    std::vector<SgDirectedGraphEdge*> getInEdges(SgGraphNode* node);

    // Provide the same interface to get the beginning/end graph node for a SgNode
    SgGraphNode* cfgForBeginning(SgNode* node);
    SgGraphNode* cfgForEnd(SgNode* node);

    //! Get the index of a CFG node.
    static int getIndex(SgGraphNode* node);

    //! Output the graph to a DOT file.
    void cfgToDot(SgNode* node, const std::string& file_name);

protected:
    //void buildCFG(CFGNode n);
    template <class NodeT, class EdgeT>
    void buildCFG(NodeT n, std::map<NodeT, SgGraphNode*>& all_nodes, std::set<NodeT>& explored);

    //! Delete all nodes and edges in the graph and release memories.
    void clearNodesAndEdges();

    //! Turn a graph node into a CFGNode which is defined in VirtualCFG namespace.
    CFGNode toCFGNode(SgGraphNode* node);

    // The following methods are used to build a DOT file.
    void processNodes(std::ostream & o, SgGraphNode* n, std::set<SgGraphNode*>& explored);
    void printNodePlusEdges(std::ostream & o, SgGraphNode* node);
    void printNode(std::ostream & o, SgGraphNode* node);
    void printEdge(std::ostream & o, SgDirectedGraphEdge* edge, bool isInEdge);
};


//! This class stores index of each node as an attribuite of SgGraphNode.
class CFGNodeAttribute : public AstAttribute
{
    int index_;
    SgIncidenceDirectedGraph* graph_;

public:
    CFGNodeAttribute(int idx = 0, SgIncidenceDirectedGraph* graph = NULL) 
        : index_(idx), graph_(graph) {}

    int getIndex() const { return index_; }

    void setIndex(int idx) { index_ = idx; }

    const SgIncidenceDirectedGraph* getGraph() const { return graph_; }
    SgIncidenceDirectedGraph* getGraph() { return graph_; }

    void setGraph(SgIncidenceDirectedGraph* graph)
    { graph_ = graph; }
};

template <class EdgeT>
class CFGEdgeAttribute : public AstAttribute
{
    EdgeT edge_;
public:
    CFGEdgeAttribute(const EdgeT& e) : edge_(e) {}

    void setEdge(const EdgeT& e)
    { edge_ = e; }
    
    EdgeT getEdge() const
    { return edge_; }
};

// The following are some auxiliary functions, since SgGraphNode cannot provide them.
std::vector<SgDirectedGraphEdge*> outEdges(SgGraphNode* node);
std::vector<SgDirectedGraphEdge*> inEdges(SgGraphNode* node);

} // end of namespace StaticCFG

#endif
