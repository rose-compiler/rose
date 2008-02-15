#ifndef COMPSLICE_LOCALITYANAL
#define COMPSLICE_LOCALITYANAL
#include <CompSlice.h>
#include <LoopTreeLocality.h>
#include <iostream>
#include <sstream>

class CompSliceLocalityAnal
{
  LoopTreeLocalityAnal &anal;
  unsigned linesize, reuseDist;
 public:
  typedef LoopTreeLocalityAnal::AstNodeSet AstNodeSet;
  CompSliceLocalityAnal( LoopTreeLocalityAnal &a, unsigned ls, unsigned _reuseDist)
        : anal(a), linesize(ls), reuseDist(_reuseDist) {}
  int TemporaryReuses(const CompSlice *slice1, const CompSlice *slice2,
                      AstNodeSet& refSet);
  int SpatialReuses(const CompSlice *slice1, const CompSlice *slice2,
                      AstNodeSet& refSet);
  float SpatialReuses( const CompSlice *slice1);
  unsigned GetCacheLineSize() const { return linesize; }
};

class CompSliceLocalityRegistry : protected CompSliceLocalityAnal
{
  class Impl;
 public:
  class RelInitInfo
  { public:
    const CompSlice *slice1, *slice2;
    CompSliceLocalityAnal& anal;
    RelInitInfo(CompSliceLocalityAnal& a, const CompSlice *s1,
               const CompSlice *s2)
     : slice1(s1), slice2(s2), anal(a) {}
  };
  class SliceSelfInfo;
  class SliceRelInfo
  {
    CompSliceLocalityAnal::AstNodeSet tmpRefSet, spRefSet;
   public:
    SliceRelInfo( RelInitInfo info)
     {  
        info.anal.TemporaryReuses(info.slice1, info.slice2, tmpRefSet); 
        info.anal.SpatialReuses(info.slice1, info.slice2, spRefSet); 
     }
    void FuseRelInfo( const SliceRelInfo& rel)
     { tmpRefSet.insert(rel.tmpRefSet.begin(), rel.tmpRefSet.end()); }
    int TemporaryReuses( CompSliceLocalityAnal::AstNodeSet* refset = 0) const 
         { 
            if (refset != 0)
               *refset = tmpRefSet;
            return tmpRefSet.size();
         }
    int SpatialReuses() const { return spRefSet.size(); }
    std::string ToString() const
    {  
       std::stringstream out;
       out << " Temporary reuse: " << TemporaryReuses(); 
       return out.str();
    }
    friend class CompSliceLocalityRegistry::SliceSelfInfo;
  };

  class SelfInitInfo
  { public:
   const CompSlice *slice;
   CompSliceLocalityAnal& anal;
   SelfInitInfo(CompSliceLocalityAnal& a, const CompSlice *s)
    : slice(s), anal(a) {}
  };

  class SliceSelfInfo
  {
   float spatialReuses;
   CompSliceLocalityAnal::AstNodeSet tmpRefSet;
  public:
   SliceSelfInfo(SelfInitInfo info)
   { spatialReuses = info.anal.SpatialReuses(info.slice);
     info.anal.TemporaryReuses(info.slice, info.slice, tmpRefSet);
   }
   void FuseSelfInfo( const SliceSelfInfo& that, const SliceRelInfo& rel)
   { spatialReuses += that.spatialReuses;
     tmpRefSet.insert( that.tmpRefSet.begin(), that.tmpRefSet.end());
     tmpRefSet.insert( rel.tmpRefSet.begin(), rel.tmpRefSet.end());
   }
   int TemporaryReuses( CompSliceLocalityAnal::AstNodeSet* refset = 0) const 
         { 
            if (refset != 0)
               *refset = tmpRefSet;
            return tmpRefSet.size();
         }
   float SpatialReuses() { return spatialReuses; }
   std::string ToString() const
    {  
       std::stringstream out;
       out << "spatial reuse: " << spatialReuses << "; Temporary reuse: " <<
              TemporaryReuses();
       return out.str();
    }
  };

  class CreateInitInfo
  {
    CompSliceLocalityAnal& anal;
  public:
   CreateInitInfo(CompSliceLocalityAnal& a) : anal(a) {}
   SelfInitInfo operator()(const CompSlice *slice)
    { return SelfInitInfo(anal, slice); }
   RelInitInfo operator()(const CompSlice* slice1, const CompSlice* slice2)
    { return RelInitInfo( anal, slice1, slice2); }
  };

 private:
  Impl* impl;
  void operator = (const CompSliceLocalityRegistry& that) {}
 public:
  typedef LoopTreeLocalityAnal::AstNodeSet AstNodeSet;
  CompSliceLocalityRegistry( LoopTreeLocalityAnal &a, unsigned ls, unsigned reuseDist) ;
  ~CompSliceLocalityRegistry();
  int TemporaryReuses(const CompSlice *slice1, const CompSlice *slice2,
                      CompSliceLocalityAnal::AstNodeSet* refset = 0) ;
  int SpatialReuses(const CompSlice *slice1, const CompSlice *slice2);
  int TemporaryReuses(const CompSlice* slice);
  float SpatialReuses( const CompSlice *slice);
};

#endif
