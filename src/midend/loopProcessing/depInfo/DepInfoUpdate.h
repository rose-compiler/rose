#ifndef UPDATE_DEPINFO
#define UPDATE_DEPINFO

#include <depInfo/DepInfoSet.h>
#include <depInfo/DomainInfo.h>

template <class Update>
class UpdateDepInfo {
  Update T;
 public:
  UpdateDepInfo( Update _T) : T(_T) {}
//Boolean operator() ( DepInfo &d, DepDirection dir)
  int operator() ( DepInfo &d, DepDirection dir)
     { return T(d, dir); }
//Boolean operator()( DepInfoSet &infoset, DepDirection dir)
  int operator()( DepInfoSet &infoset, DepDirection dir)
   {  UpdateEach( infoset.GetUpdateIterator(),
                  UpdateDepInfo1(T, dir)); 
      return infoset.NumOfDeps() > 0;
   }
};


class DepInfoInsertLoop 
{
  int level;
 public:
  DepInfoInsertLoop( int _level) { level = _level; }
//Boolean operator ()( DepInfo &d, DepDirection dir)
  int operator ()( DepInfo &d, DepDirection dir)
     { d.InsertLoop(level, dir); return true; }
};

class DepInfoMergeLoop 
{
  int srcIndex, desIndex;
 public:
  DepInfoMergeLoop(int _srcIndex, int _desIndex) 
        { srcIndex = _srcIndex; desIndex = _desIndex; }
//Boolean operator ()( DepInfo &d, DepDirection dir)
  int operator ()( DepInfo &d, DepDirection dir)
    { d.MergeLoop(srcIndex, desIndex, dir);  return true; }
};

class DepInfoSwapLoop 
{
  int srcIndex, desIndex;
 public:
  DepInfoSwapLoop(int _srcIndex, int _desIndex)
        { srcIndex = _srcIndex; desIndex = _desIndex; }
//Boolean operator ()( DepInfo &d, DepDirection dir)
  int operator ()( DepInfo &d, DepDirection dir)
     { d.SwapLoop(srcIndex, desIndex, dir);  return true; }
};

class DepInfoRemoveLoop 
{
  int level;
 public:
  DepInfoRemoveLoop( int _level) { level = _level; }
//Boolean operator ()( DepInfo &d, DepDirection dir)
  int operator ()( DepInfo &d, DepDirection dir)
     { d.RemoveLoop(level, dir); return true; }
};

class DepInfoAlignLoop 
{
   int index, align;
 public:
    DepInfoAlignLoop(int _level, int _align) 
      { index = _level; align = _align; }
//Boolean operator ()( DepInfo &d, DepDirection dir)
  int operator ()( DepInfo &d, DepDirection dir)
     { d.AlignLoop(index, align, dir);  return true; }
};

class DepInfoRestrictDomain
{
   const DomainCond &cond;
 public:
   DepInfoRestrictDomain( const DomainCond &c) : cond(c) {}
// Boolean operator ()( DepInfo &dep, DepDirection dir) 
   int operator ()( DepInfo &dep, DepDirection dir) 
     { cond.RestrictDepInfo( dep, dir); 
       if (dep.IsTop())
            return false;
       return true;
     }
};

#endif
