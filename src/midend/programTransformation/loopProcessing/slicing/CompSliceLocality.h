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
  class SliceRelInfo
  {
    CompSliceLocalityAnal::AstNodeSet tmpRefSet, spRefSet;
   public:
    SliceRelInfo( CompSliceLocalityAnal& anal, 
                  const CompSlice* slice1, const CompSlice* slice2)
     {  
        anal.TemporaryReuses(slice1, slice2, tmpRefSet); 
        anal.SpatialReuses(slice1, slice2, spRefSet); 
     }
    const CompSliceLocalityAnal::AstNodeSet* get_temporaryReuseSet() const
       { return &tmpRefSet; }
    void FuseRelInfo( const SliceRelInfo& rel)
     { tmpRefSet.insert(rel.tmpRefSet.begin(), rel.tmpRefSet.end()); }
    int TemporaryReuses( CompSliceLocalityAnal::AstNodeSet* refset = 0) const 
         { 
            if (refset != 0)
               *refset = tmpRefSet;
            return tmpRefSet.size();
         }
    int SpatialReuses() const { return spRefSet.size(); }
    std::string toString() const
    {  
       std::stringstream out;
       out << " Temporary reuse: " << TemporaryReuses(); 
       return out.str();
    }
  };

  class SliceSelfInfo
  {
   float spatialReuses;
   CompSliceLocalityAnal::AstNodeSet tmpRefSet;
  public:
   SliceSelfInfo (CompSliceLocalityAnal& anal, const CompSlice *slice)
   { 
     spatialReuses = anal.SpatialReuses(slice);
     anal.TemporaryReuses(slice, slice, tmpRefSet);
   }
   void FuseSelfInfo( const SliceSelfInfo& that, const SliceRelInfo& rel)
   { spatialReuses += that.spatialReuses;
     const CompSliceLocalityAnal::AstNodeSet* 
                 tmpSet1 = rel.get_temporaryReuseSet();
     tmpRefSet.insert( that.tmpRefSet.begin(), that.tmpRefSet.end());
     tmpRefSet.insert( tmpSet1->begin(), tmpSet1->end());
   }
   int TemporaryReuses( CompSliceLocalityAnal::AstNodeSet* refset = 0) const 
         { 
            if (refset != 0)
               *refset = tmpRefSet;
            return tmpRefSet.size();
         }
   float SpatialReuses() { return spatialReuses; }
   std::string toString() const
    {  
       std::stringstream out;
       out << "spatial reuse: " << spatialReuses << "; Temporary reuse: " <<
              TemporaryReuses();
       return out.str();
    }
  };

 private:
  Impl* impl;
  void operator = (const CompSliceLocalityRegistry &) {}

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
