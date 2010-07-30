#ifndef INTERPROCEDURAL_CFG_H 
#define INTERPROCEDURAL_CFG_H 

#include "staticCFG.h"
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


class InterproceduralCFG : public CFG
{

public:
    InterproceduralCFG() : CFG() {}

    // The valid nodes are SgProject, SgStatement, SgExpression and SgInitializedName
    InterproceduralCFG(SgNode* node, bool is_filtered = false) 
      : CFG(node, is_filtered) {}

    ~InterproceduralCFG() 
    { clearNodesAndEdges(); }

    // Build CFG for debugging.
    void buildFullCFG();
    // Build filtered CFG which only contains interesting nodes.
    void buildFilteredCFG();

    template <class NodeT, class EdgeT>
    void buildCFG(NodeT n, std::map<NodeT, SgGraphNode*>& all_nodes, std::set<NodeT>& explored);
};

} // end of namespace StaticCFG

#endif
