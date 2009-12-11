#ifndef UPDATE_DEPINFO
#define UPDATE_DEPINFO

#include <DepInfoSet.h>
#include <DomainInfo.h>

template <class Update>
class UpdateDepInfo {
  Update T;
 public:
  UpdateDepInfo( Update _T) : T(_T) {}
  bool operator() ( DepInfo &d, DepDirection dir)
     { return T(d, dir); }
  bool operator()( DepInfoSet &infoset, DepDirection dir)
   {  
     DepInfoUpdateIterator p = infoset.GetUpdateIterator();
     UpdateEach(p, UpdateDepInfo1(T, dir)); 
      return infoset.NumOfDeps() > 0;
   }
};


class DepInfoInsertLoop 
{
  int level;
 public:
  DepInfoInsertLoop( int _level) { level = _level; }
  bool operator ()( DepInfo &d, DepDirection dir)
     { d.InsertLoop(level, dir); return true; }
};

class DepInfoMergeLoop 
{
  int srcIndex, desIndex;
 public:
  DepInfoMergeLoop(int _srcIndex, int _desIndex) 
        { srcIndex = _srcIndex; desIndex = _desIndex; }
  bool operator ()( DepInfo &d, DepDirection dir)
    { d.MergeLoop(srcIndex, desIndex, dir);  return true; }
};

class DepInfoSwapLoop 
{
  int srcIndex, desIndex;
 public:
  DepInfoSwapLoop(int _srcIndex, int _desIndex)
        { srcIndex = _srcIndex; desIndex = _desIndex; }
  bool operator ()( DepInfo &d, DepDirection dir)
     { d.SwapLoop(srcIndex, desIndex, dir);  return true; }
};

class DepInfoRemoveLoop 
{
  int level;
 public:
  DepInfoRemoveLoop( int _level) { level = _level; }
  bool operator ()( DepInfo &d, DepDirection dir)
     { d.RemoveLoop(level, dir); return true; }
};

class DepInfoAlignLoop 
{
   int index, align;
 public:
    DepInfoAlignLoop(int _level, int _align) 
      { index = _level; align = _align; }
  bool operator ()( DepInfo &d, DepDirection dir)
     { d.AlignLoop(index, align, dir);  return true; }
};

class DepInfoRestrictDomain
{
   const DomainCond &cond;
 public:
   DepInfoRestrictDomain( const DomainCond &c) : cond(c) {}
   bool operator ()( DepInfo &dep, DepDirection dir) 
     { cond.RestrictDepInfo( dep, dir); 
       if (dep.IsTop())
            return false;
       return true;
     }
};

#endif
