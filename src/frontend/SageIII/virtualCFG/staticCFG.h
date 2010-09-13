#ifndef STATIC_CFG_H 
#define STATIC_CFG_H 

#if 0
#include <AstInterface.h>
#include <GraphDotOutput.h>
#include <VirtualGraphCreate.h>
#endif
//#include <rose.h>
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
    SgIncidenceDirectedGraph* graph_;
    std::map<CFGNode, SgGraphNode*> all_nodes_;
    SgNode* start_;
    bool is_filtered_;

public:
    CFG() : graph_(NULL), start_(NULL) {}

    // The valid nodes are SgProject, SgStatement, SgExpression and SgInitializedName
    CFG(SgNode* node, bool is_filtered = false)
        : graph_(NULL), start_(node), is_filtered_(is_filtered)
    {
        buildCFG();
    }

    virtual ~CFG()
    { clearNodesAndEdges(); }

    // Set the start node for graph building. 
    // The valid nodes are SgProject, SgStatement, SgExpression and SgInitializedName
    void setStart(SgNode* node) { start_ = node; }

    bool isFilteredCFG() const { return is_filtered_; }
    void setFiltered(bool flag) { is_filtered_ = flag; }


    // Build CFG according to the 'is_filtered_' flag.
    void buildCFG()
    {
        if (is_filtered_) buildFilteredCFG();
        else buildFullCFG();
    }

    // Build CFG for debugging.
    virtual void buildFullCFG();
    // Build filtered CFG which only contains interesting nodes.
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

    // Get the index of a CFG node.
    int getIndex(SgGraphNode* node);

    // Output the graph to a DOT file.
    void cfgToDot(SgNode* node, const std::string& file_name)
    {
        std::ofstream ofile(file_name.c_str(), std::ios::out);
        ofile << "digraph defaultName {\n";
        std::set<SgGraphNode*> explored;
        processNodes(ofile, cfgForBeginning(node), explored);
        ofile << "}\n";
    }

protected:
    //void buildCFG(CFGNode n);
    template <class NodeT, class EdgeT>
    void buildCFG(NodeT n, std::map<NodeT, SgGraphNode*>& all_nodes, std::set<NodeT>& explored);
    void clearNodesAndEdges();

    // Turn a graph node into a CFGNode which is defined in VirtualCFG namespace.
    CFGNode toCFGNode(SgGraphNode* node);

    // The following methods are used to build a DOT file.
    void processNodes(std::ostream & o, SgGraphNode* n, std::set<SgGraphNode*>& explored);
    void printNodePlusEdges(std::ostream & o, SgGraphNode* node);
    void printNode(std::ostream & o, SgGraphNode* node);
    void printEdge(std::ostream & o, SgDirectedGraphEdge* edge, bool isInEdge);
};


// This class is to store index of each node as an attribuite of SgGraphNode.
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
