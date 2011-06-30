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

typedef std::vector<SgInitializedName*> VarName;

struct PathInfo : std::map<int, PathSet>
{
    bool hasPath(int dagIdx, int pathIdx) const
    {
        std::map<int, PathSet>::const_iterator iter = find(dagIdx);
        if (iter != end())
            return iter->second[pathIdx];
        return false;
    }

    std::string toString() const 
    {
        std::string str;
        for (const_iterator iter = begin(), iterEnd = end();
                iter != iterEnd; ++iter)
        {
            std::string s;
            boost::to_string(iter->second, s);
            s = boost::lexical_cast<std::string>(iter->first) + ":" + s;
            str += "\\n" + s;
        }
        return str;
    }
};

PathInfo operator&(const PathInfo& path1, const PathInfo& path2);
PathInfo operator|(const PathInfo& path1, const PathInfo& path2);

} // end of Backstroke


#endif	/* BACKSTROKE_VALUEGRAPH_TYPES_H */
