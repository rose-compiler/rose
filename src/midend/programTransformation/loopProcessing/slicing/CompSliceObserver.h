
#ifndef COMPSLICE_OBSERVER
#define COMPSLICE_OBSERVER

#include <ObserveObject.h>
#include <CompSlice.h>

class CompSliceFuseInfo;
class CompSliceDeleteInfo;
class CompSliceAlignInfo;
class CompSliceObserver
{
  public:
   virtual void UpdateDelete( const CompSlice &slice) {};
   virtual void UpdateFusion( const CompSliceFuseInfo &info) {}
   virtual void UpdateAlign( const CompSliceAlignInfo &info) {}
   virtual void Dump() const {}
   virtual ~CompSliceObserver() {}
};

class CompSliceObserveInfo  : public ObserveInfo<CompSliceObserver>
{
   const CompSlice& slice;
 public:
  CompSliceObserveInfo(const CompSlice &s) : slice(s) {}
  const CompSlice& GetSlice() const { return slice; }
  virtual void UpdateObserver( CompSliceObserver &o) const= 0; 
};
   
class CompSliceFuseInfo : public CompSliceObserveInfo
{
  const CompSlice &that;
 public:
  CompSliceFuseInfo( const CompSlice &c1, const CompSlice &c2) 
    : CompSliceObserveInfo(c1), that(c2) {}
  const CompSlice& GetSlice2() const { return that; }
  virtual void UpdateObserver( CompSliceObserver &o) const 
             { o.UpdateFusion(*this);}
};

class CompSliceAlignInfo : public CompSliceObserveInfo
{
  int align;
 public:
  CompSliceAlignInfo( const CompSlice &c1, int a)
    : CompSliceObserveInfo(c1), align(a) {}
  int GetAlign() const { return align; }
  virtual void UpdateObserver( CompSliceObserver &o) const 
             { o.UpdateAlign(*this);}
};

class CompSliceDeleteInfo : public CompSliceObserveInfo
{
 public:
  CompSliceDeleteInfo( const CompSlice &c) : CompSliceObserveInfo(c) {}
  virtual void UpdateObserver( CompSliceObserver &o) const
     { o.UpdateDelete(GetSlice());}
};

class CompSliceNestSwapInfo;
class CompSliceNestDeleteEntryInfo;
class CompSliceNestDuplicateEntryInfo;
class CompSliceNestDeleteInfo;
class CompSliceNestAlignEntryInfo;
class CompSliceNestFusionInfo;
class CompSliceNestObserver
{
 public:
  virtual void UpdateDelete( const CompSliceNest&) {};
  virtual void UpdateDeleteEntry( const CompSliceNestDeleteEntryInfo&) {}
  virtual void UpdateAlignEntry( const CompSliceNestAlignEntryInfo&) {}
  virtual void UpdateDuplicateEntry(const CompSliceNestDuplicateEntryInfo&) {}
  virtual void UpdateSwap( const CompSliceNestSwapInfo&) {}
  virtual void UpdateFusion( const CompSliceNestFusionInfo&) {}
  virtual void Dump() const {}
  virtual ~CompSliceNestObserver() {}
};

class CompSliceNestObserveInfo  : public ObserveInfo<CompSliceNestObserver>
{
   const CompSliceNest &sliceVec;
 public:
  CompSliceNestObserveInfo(const CompSliceNest& v) : sliceVec(v) {}
  const CompSliceNest& GetSliceVec() const { return sliceVec; }
  virtual void UpdateObserver( CompSliceNestObserver &o) const = 0;
};

class CompSliceNestSwapInfo : public CompSliceNestObserveInfo
{
  int index1, index2;
 public:
  CompSliceNestSwapInfo( const CompSliceNest& c, int i1, int i2)
    : CompSliceNestObserveInfo(c), index1(i1), index2(i2) {}
  int GetIndex1() const { return index1; }
  int GetIndex2() const { return index2; }
  virtual void UpdateObserver( CompSliceNestObserver &o) const 
         { o.UpdateSwap(*this);}
};

class CompSliceNestDeleteEntryInfo : public CompSliceNestObserveInfo
{
  int index;
  bool saveAsInner;
 public:
  CompSliceNestDeleteEntryInfo( const CompSliceNest& c, int i, bool _save)
    : CompSliceNestObserveInfo(c), index(i), saveAsInner(_save) {}
  int GetIndex() const { return index; }
  bool SaveAsInner() const {return saveAsInner; }
  virtual void UpdateObserver( CompSliceNestObserver &o) const
      { o.UpdateDeleteEntry(*this);}
};

class CompSliceNestAlignEntryInfo : public CompSliceNestObserveInfo
{
  int index, align;
 public:
  CompSliceNestAlignEntryInfo( const CompSliceNest& c, int i, int a)
    : CompSliceNestObserveInfo(c), index(i), align(a) {}
  int GetIndex() const { return index; }
  int GetAlign() const { return align; }
  virtual void UpdateObserver( CompSliceNestObserver &o) const
      { o.UpdateAlignEntry(*this);}
};

class CompSliceNestFusionInfo : public CompSliceNestObserveInfo
{
  const CompSliceNest& that;
 public:
  CompSliceNestFusionInfo( const CompSliceNest& c, const CompSliceNest& c1)
    : CompSliceNestObserveInfo(c), that(c1) {}
  const CompSliceNest& GetSliceNest2() const { return that; }
  virtual void UpdateObserver( CompSliceNestObserver &o) const
      { o.UpdateFusion(*this);}
};

class CompSliceNestDuplicateEntryInfo : public CompSliceNestObserveInfo
{
  int desc, src;
 public:
  CompSliceNestDuplicateEntryInfo( const CompSliceNest& c, int d, int s)
    : CompSliceNestObserveInfo(c), desc(d),src(s) {}
  int GetDescIndex() const { return desc; }
  int GetSrcIndex() const { return src; }
  virtual void UpdateObserver( CompSliceNestObserver &o) const
      { o.UpdateDuplicateEntry(*this);}
};

class CompSliceNestDeleteInfo : public CompSliceNestObserveInfo
{
 public:
  CompSliceNestDeleteInfo( const CompSliceNest &c) 
        : CompSliceNestObserveInfo(c) {}
  virtual void UpdateObserver( CompSliceNestObserver &o) const
     { o.UpdateDelete(GetSliceVec());}
};

#endif
