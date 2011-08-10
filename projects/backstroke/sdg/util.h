#ifndef SYSTEM_DEPENDENCE_GRAPH_UTIL_H
#define	SYSTEM_DEPENDENCE_GRAPH_UTIL_H

#include "newCFG.h"

namespace SDG
{

typedef std::map<
    ControlFlowGraph::Vertex,
    std::map<ControlFlowGraph::Vertex, 
            std::vector<ControlFlowGraph::Edge> > >
DominanceFrontiersT;

//! Build dominance frontiers for all nodes in the given CFG.
DominanceFrontiersT buildDominanceFrontiers(const ControlFlowGraph& cfg);

//! Check if a parameter of the given type in a function call is passed by reference or not.
bool isParaPassedByRef(SgType* type);

} // end of namespace SystemDependenceGraph



#endif	/* SYSTEM_DEPENDENCE_GRAPH_UTIL_H */

