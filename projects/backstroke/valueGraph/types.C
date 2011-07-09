#include "types.h"

namespace Backstroke
{

using namespace std;

#define foreach         BOOST_FOREACH

PathInfos operator&(const PathInfos& path1, const PathInfos& path2)
{
    PathInfos paths;
    PathInfos::const_iterator
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
            paths[first1->first].paths = first1->second.paths & first2->second.paths;
            ++first1; ++first2; 
        }
    }
    return paths;
}

PathInfos operator|(const PathInfos& path1, const PathInfos& path2)
{
    PathInfos paths = path1;
    
    foreach (const PathInfos::value_type& idxPaths, path2)
    {
        PathInfos::iterator iter = paths.find(idxPaths.first);
        if (iter != paths.end())
            iter->second.paths |= idxPaths.second.paths;
        else
            paths.insert(iter, idxPaths);
    }
    
    return paths;
}


} // end of namespace Backstroke
