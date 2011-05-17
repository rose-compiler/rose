#ifndef BACKSTROKE_VALUEGRAPH_TYPES_H
#define	BACKSTROKE_VALUEGRAPH_TYPES_H

#include "CFGFilter.h"
#include <slicing/backstrokeCFG.h>
#include <slicing/backstrokeCDG.h>
#include <ssa/dataflowCfgFilter.h>
#include <boost/dynamic_bitset.hpp>

#define VG_DEBUG 

namespace Backstroke
{

//typedef CFG<BackstrokeCFGNodeFilter> BackstrokeCFG;
typedef CFG<ssa_private::DataflowCfgFilter> BackstrokeCFG;
typedef CDG<BackstrokeCFG> BackstrokeCDG;
//typedef FilteredCFG BackstrokeCFG;

typedef boost::adjacency_list<boost::vecS, boost::vecS, boost::bidirectionalS,
		BackstrokeCFG::Vertex, BackstrokeCFG::Edge> DAG;

typedef boost::dynamic_bitset<> PathSet;

} // end of Backstroke


#endif	/* BACKSTROKE_VALUEGRAPH_TYPES_H */
