
#ifndef FUSIONINTERFACE_H
#define FUSIONINTERFACE_H

#include <slicing/CompSliceDepGraph.h>
#include <LoopTransformOptions.h>
class CompSliceNest;
class DepInfo;
class CompSliceLocalityRegistry;

struct FusionInfo {
//Boolean succ;
  int succ;
  int align ;
//FusionInfo(Boolean s = false, int a=0) : succ (s), align(a) {}
  FusionInfo(int s = false, int a=0) : succ (s), align(a) {}
//operator Boolean() { return succ; }
  operator int() { return succ; }
};
FusionInfo GetFusionInfo(const DepInfo &info, int index1, int index2);
void FuseSliceNests( CompSliceNest& g1, CompSliceNest& g2, const DepInfo &info);

class LoopFusionAnal
{
 public:
  virtual FusionInfo operator()(CompSliceLocalityRegistry *anal, CompSliceNest& n1, CompSliceNest& n2,
                                int j, int k, const DepInfo& e);
  virtual ~LoopFusionAnal() {}
  virtual LoopTransformOptions::OptType GetOptimizationType() = 0;
};

class OrigLoopFusionAnal : public LoopFusionAnal
{
 public:
  FusionInfo operator()(CompSliceLocalityRegistry *anal, CompSliceNest& n1, CompSliceNest& n2,
                                int j, int k, const DepInfo& e);
  virtual LoopTransformOptions::OptType GetOptimizationType() { return LoopTransformOptions::NO_OPT; }
};

class InnermostLoopFision : public OrigLoopFusionAnal
{
  public:
   FusionInfo operator()(CompSliceLocalityRegistry *anal, CompSliceNest& n1, CompSliceNest& n2,
                                int j, int k, const DepInfo& e);
   virtual LoopTransformOptions::OptType GetOptimizationType() { return LoopTransformOptions::INNER_MOST_OPT; }
};


class AnyReuseFusionAnal : public LoopFusionAnal
{
 public:
  FusionInfo operator()( CompSliceLocalityRegistry *anal, CompSliceNest& n1, CompSliceNest& n2,
                                int j, int k, const DepInfo& e);
  virtual LoopTransformOptions::OptType GetOptimizationType() { return LoopTransformOptions::MULTI_LEVEL_OPT; }
};

class BetterReuseFusionAnal : public LoopFusionAnal
{
  int index;
 public:
  BetterReuseFusionAnal() { index = 0; }
  FusionInfo operator()( CompSliceLocalityRegistry *anal, CompSliceNest& n1, CompSliceNest& n2,
                                int j, int k, const DepInfo& e);
  virtual LoopTransformOptions::OptType GetOptimizationType() { return LoopTransformOptions::MULTI_LEVEL_OPT; }
};

class LoopNestFusion 
{
 public:
  virtual ~LoopNestFusion() {}
//virtual Boolean Fusible(CompSliceLocalityRegistry *anal,CompSliceNest &n1,CompSliceNest &n2,
  virtual int Fusible(CompSliceLocalityRegistry *anal,CompSliceNest &n1,CompSliceNest &n2,
                           const DepInfo &e) const { return false; }
  virtual void Fuse( CompSliceLocalityRegistry *reg, CompSliceNest &n1, CompSliceNest &n2, 
                     DepInfo &e) const { assert(false); }
  virtual LoopTransformOptions::OptType GetOptimizationType() { return LoopTransformOptions::MULTI_LEVEL_OPT; }
};

class MultiLevelFusion : public LoopNestFusion
{
  LoopFusionAnal *anal;
 public:
  MultiLevelFusion( LoopFusionAnal *a) : anal(a) {} 
  ~MultiLevelFusion() { delete anal; }
//Boolean Fusible( CompSliceLocalityRegistry *anal, CompSliceNest &n1,CompSliceNest &n2, 
  int Fusible( CompSliceLocalityRegistry *anal, CompSliceNest &n1,CompSliceNest &n2, 
                   const DepInfo &e) const;
  void Fuse( CompSliceLocalityRegistry *reg, CompSliceNest &n1, CompSliceNest &n2, 
             DepInfo &e) const;
  virtual LoopTransformOptions::OptType GetOptimizationType() { return anal->GetOptimizationType(); }
};

class SameLevelFusion : public LoopNestFusion
{
  LoopFusionAnal* anal;
 public:
  SameLevelFusion( LoopFusionAnal* a) : anal(a) {}
  ~SameLevelFusion() { delete anal; }
//Boolean Fusible( CompSliceLocalityRegistry *anal, CompSliceNest &n1, CompSliceNest &n2, 
  int Fusible( CompSliceLocalityRegistry *anal, CompSliceNest &n1, CompSliceNest &n2, 
                   const DepInfo &e) const;
  void Fuse( CompSliceLocalityRegistry *reg, CompSliceNest &n1, CompSliceNest &n2, 
             DepInfo &e) const;
  virtual LoopTransformOptions::OptType GetOptimizationType() { return anal->GetOptimizationType(); }
};

void SliceNestTypedFusion(  CompSliceLocalityRegistry *anal,CompSliceDepGraphCreate &t, 
                           const LoopNestFusion& fuse );
void SliceNestReverseTypedFusion(  CompSliceLocalityRegistry *anal, CompSliceDepGraphCreate &t, 
                                 const LoopNestFusion& fuse);

#endif
