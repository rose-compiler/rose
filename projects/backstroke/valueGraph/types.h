#ifndef BACKSTROKE_VALUEGRAPH_TYPES_H
#define	BACKSTROKE_VALUEGRAPH_TYPES_H

#include "CFGFilter.h"
#include <slicing/backstrokeCFG.h>
#include <slicing/backstrokeCDG.h>
#include <dataflowCfgFilter.h>
#include <boost/dynamic_bitset.hpp>

#define VG_DEBUG 

namespace Backstroke
{

//typedef CFG<BackstrokeCFGNodeFilter> BackstrokeCFG;
typedef CFG<ssa_private::DataflowCfgFilter> BackstrokeCFG;
typedef CDG<BackstrokeCFG> BackstrokeCDG;
//typedef FilteredCFG BackstrokeCFG;

typedef BackstrokeCDG::ControlDependences ControlDependences;

//typedef boost::adjacency_list<boost::vecS, boost::vecS, boost::bidirectionalS,
//		BackstrokeCFG::Vertex, BackstrokeCFG::Edge> DAG;

typedef boost::dynamic_bitset<> PathSet;

typedef std::vector<SgInitializedName*> VarName;

struct PathInfo : PathSet
{
    PathInfo() {}
    PathInfo(const PathSet& paths) : PathSet(paths) {}
    
    //PathSet paths;
    std::vector<std::vector<std::pair<int, int> > > pathCond;
    
    bool isEmpty() const { return !any(); }
    
    PathInfo operator&(const PathInfo& p) const;
    
    PathInfo& operator&=(const PathInfo& p);
    PathInfo& operator|=(const PathInfo& p);
    PathInfo& operator-=(const PathInfo& p);
};

std::ostream& operator<<(std::ostream& os, const PathInfo& path);

//inline bool operator<(const PathInfo& p1, const PathInfo& p2)
//{ return paths < p2.paths; }

struct PathInfos : std::map<int, PathInfo>
{
    bool hasPath(int dagIdx, int pathIdx) const;

    std::string toString() const;
};

PathInfos operator&(const PathInfos& path1, const PathInfos& path2);
PathInfos operator|(const PathInfos& path1, const PathInfos& path2);

} // end of Backstroke


#endif	/* BACKSTROKE_VALUEGRAPH_TYPES_H */
