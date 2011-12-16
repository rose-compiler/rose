/* 
 * File:   util.h
 * Author: Cong Hou [hou_cong@gatech.edu]
 */

#ifndef SYSTEM_DEPENDENCE_GRAPH_UTIL_H
#define SYSTEM_DEPENDENCE_GRAPH_UTIL_H

#include "staticCFG.h"

namespace SDG
{

typedef std::map<
    StaticCFG::ControlFlowGraph::Vertex,
    std::map<StaticCFG::ControlFlowGraph::Vertex, 
            std::vector<StaticCFG::ControlFlowGraph::Edge> > >
DominanceFrontiersT;

//! Build dominance frontiers for all nodes in the given CFG.
DominanceFrontiersT buildDominanceFrontiers(const StaticCFG::ControlFlowGraph& cfg);

//! Check if a parameter of the given type in a function call is passed by reference or not.
bool isParaPassedByRef(SgType* type);

} // end of namespace SystemDependenceGraph



#endif /* SYSTEM_DEPENDENCE_GRAPH_UTIL_H */

