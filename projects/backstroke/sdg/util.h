#ifndef SYSTEM_DEPENDENCE_GRAPH_UTIL_H
#define	SYSTEM_DEPENDENCE_GRAPH_UTIL_H

#include "newCFG.h"

namespace SystemDependenceGraph
{

typedef std::map<
    ControlFlowGraph::Vertex,
    std::map<ControlFlowGraph::Vertex, 
            std::vector<ControlFlowGraph::Edge> > >
DominanceFrontiersT;

//! Build dominance frontiers for all nodes in the given CFG.
DominanceFrontiersT buildDominanceFrontiers(const ControlFlowGraph& cfg);


} // end of namespace SystemDependenceGraph



#endif	/* SYSTEM_DEPENDENCE_GRAPH_UTIL_H */

