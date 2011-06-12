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

inline PathInfo operator&(const PathInfo& path1, const PathInfo& path2)
{
    PathInfo paths;
    std::map<int, PathSet>::const_iterator
        first1 = path1.begin(),
        last1 = path1.end(),
        first2 = path2.begin(),
        last2 = path2.end();

    while (first1!=last1 && first2!=last2)
    {
        if (first1->first < first2->first) ++first1;
        else if (first2->first < first1->first) ++first2;
        else 
        {
            paths[first1->first] = first1->second & first2->second;
            ++first1; ++first2; 
        }
    }
    return paths;
}

} // end of Backstroke


#endif	/* BACKSTROKE_VALUEGRAPH_TYPES_H */
