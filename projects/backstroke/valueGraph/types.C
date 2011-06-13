#include "types.h"

namespace Backstroke
{

using namespace std;

#define foreach         BOOST_FOREACH

PathInfo operator&(const PathInfo& path1, const PathInfo& path2)
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

PathInfo operator|(const PathInfo& path1, const PathInfo& path2)
{
    PathInfo paths = path1;
    
    foreach (const PathInfo::value_type& idxPaths, path2)
    {
        PathInfo::iterator iter = paths.find(idxPaths.first);
        if (iter != paths.end())
            iter->second |= idxPaths.second;
        else
            paths.insert(iter, idxPaths);
    }
    
    return paths;
}


} // end of namespace Backstroke
