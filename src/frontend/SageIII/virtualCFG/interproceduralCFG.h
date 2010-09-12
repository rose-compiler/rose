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
protected:
    void buildCFG(CFGNode n, 
                  std::map<CFGNode, SgGraphNode*>& all_nodes, 
                  std::set<CFGNode>& explored,
                  ClassHierarchyWrapper* classHierarchy);
public:
    InterproceduralCFG() : CFG() {}

    // The valid nodes are SgProject, SgStatement, SgExpression and SgInitializedName
    InterproceduralCFG(SgNode* node, bool is_filtered = false) 
      : CFG(node, is_filtered) {}

    ~InterproceduralCFG() { 
      clearNodesAndEdges(); 
    }

    // Build CFG for debugging.
    void buildFullCFG();
    // Build filtered CFG which only contains interesting nodes.
    void buildFilteredCFG();
};

} // end of namespace StaticCFG

#endif
