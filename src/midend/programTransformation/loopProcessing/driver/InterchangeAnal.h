
#ifndef INTERCHANGE_ANAL
#define INTERCHANGE_ANAL

#include <LoopTransformOptions.h>

//typedef enum {ORIG_ORDER, SPATIAL_REUSE_ORDER, TMP_REUSE_ORDER, 
//              MIX_REUSE_ORDER, COARSE_PAR_ORDER} LoopNestOrder;

class CompSliceNest;
class CompSliceLocalityRegistry;
class ArrangeNestingOrder 
{
 protected:
  virtual void SetNestingWeight( CompSliceLocalityRegistry *anal, 
                                 CompSliceNest &g, float *weightvec) = 0;
  void RearrangeSliceNest( CompSliceNest &g, float *weightvec);
 public:
  virtual ~ArrangeNestingOrder() {}
  void operator()( CompSliceLocalityRegistry *anal, CompSliceNest &g);
  virtual LoopTransformOptions::OptType GetOptimizationType() = 0;
};

class ArrangeOrigNestingOrder : public ArrangeNestingOrder
{
  protected:
    virtual void SetNestingWeight(CompSliceLocalityRegistry *anal, CompSliceNest& g, float *weightvec);
  public:
   virtual LoopTransformOptions::OptType GetOptimizationType() { return LoopTransformOptions::NO_OPT; }
};

class ArrangeReuseOrder : public ArrangeNestingOrder
{
  protected:
    virtual void SetNestingWeight(CompSliceLocalityRegistry *anal, CompSliceNest& g, float *weightvec);
  public:
   virtual LoopTransformOptions::OptType GetOptimizationType() { return LoopTransformOptions::PAR_LOOP_OPT; }
};

#endif
