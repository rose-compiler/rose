#ifndef UPDATE_DOMAININFO
#define UPDATE_DOMAININFO

#include <depInfo/DomainInfo.h>

class DomainCondInsertLoop 
{
  int level;
 public:
  DomainCondInsertLoop( int _level ) { level = _level; }
  void operator ()( DomainCond &d)
    { d.InsertLoop(level); }
};

class DomainCondMergeLoop 
{
  int index1,index2;
 public:
  DomainCondMergeLoop(int i1,int i2) : index1(i1), index2(i2){}
  void operator ()( DomainCond &d)
     { d.MergeLoop(index1,index2); }
};

class DomainCondSwapLoop 
{
  int index1,index2;
 public:
  DomainCondSwapLoop(int i1, int i2):index1(i1),index2(i2){}
  void operator ()( DomainCond &d)
    { d.SwapLoop(index1,index2); }
};

class DomainCondRemoveLoop 
{
  int level;
 public:
  DomainCondRemoveLoop( int _level) { level = _level; }
  void operator ()( DomainCond &d)
    { d.RemoveLoop(level); }
};

class DomainCondAlignLoop 
{
 protected:
  int index, align;
 public:
  DomainCondAlignLoop(int _level, int _align) 
      { index = _level; align = _align; }
  void operator ()( DomainCond &d)
    { d.AlignLoop(index,align); }
};

#endif
