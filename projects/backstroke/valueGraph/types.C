#include "types.h"

namespace Backstroke
{

using namespace std;

#define foreach         BOOST_FOREACH



PathInfo& PathInfo::operator&=(const PathInfo& p)
{
    if (p.empty())
    {
        this->reset();
        return *this;
    }

    if (this->is_subset_of(p))
    {
        // Empty body.
    }
    else if (p.is_subset_of(*this))
    {
        *this = p;
    }
    else
    {
        this->PathSet::operator&=(p);
        pathCond.insert(pathCond.end(), p.pathCond.begin(), p.pathCond.end());
    }
    return *this;
}


PathInfo& PathInfo::operator|=(const PathInfo& p)
{
    if (p.empty())
    {
        this->reset();
        return *this;
    }

    if (this->is_subset_of(p))
    {
        *this = p;
    }
    else if (p.is_subset_of(*this))
    {
        // Empty body.
    }
    else
    {
        this->PathSet::operator|=(p);
        //pathCond.insert(pathCond.end(), p.pathCond.begin(), p.pathCond.end());
    }
    return *this;
}

PathInfo& PathInfo::operator-=(const PathInfo& p)
{
    this->PathSet::operator-=(p);
    return *this;
}

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
            paths[first1->first] = first1->second & first2->second;
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
            iter->second |= idxPaths.second;
        else
            paths.insert(iter, idxPaths);
    }
    
    return paths;
}


} // end of namespace Backstroke
