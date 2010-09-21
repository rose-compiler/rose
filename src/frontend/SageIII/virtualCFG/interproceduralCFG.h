#ifndef INTERPROCEDURAL_CFG_H 
#define INTERPROCEDURAL_CFG_H 

#include "staticCFG.h"
#include "CallGraph.h"
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
    virtual void buildCFG(CFGNode n, 
                  std::map<CFGNode, SgGraphNode*>& all_nodes, 
                  std::set<CFGNode>& explored,
                  ClassHierarchyWrapper* classHierarchy);
public:
    InterproceduralCFG() : CFG() {}

    // The valid nodes are SgProject, SgStatement, SgExpression and SgInitializedName
    InterproceduralCFG(SgNode* node, bool is_filtered = false) 
      : CFG() {
        graph_ = NULL;
        is_filtered_ = is_filtered;
        start_ = node;
        buildCFG();
      }

    // Build CFG according to the 'is_filtered_' flag.
    virtual void buildCFG()
    {
        buildFullCFG();
    }

    // Build CFG for debugging.
    virtual void buildFullCFG();
    // Build filtered CFG which only contains interesting nodes.
    virtual void buildFilteredCFG();
};

} // end of namespace StaticCFG

#endif
