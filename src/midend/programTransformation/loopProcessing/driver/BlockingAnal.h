
#ifndef BLOCKING_ANALYSIS
#define BLOCKING_ANALYSIS

#include <CompSliceLocality.h>
#include <LoopTransformOptions.h>
#include <vector>

class LoopBlockingAnal 
{
 protected:
  std::vector<SymbolicVal> blocksize;
 public:
  SymbolicVal GetBlockSize( int index) { return blocksize[index]; }
  virtual void SetBlocking( AstInterface& fa, CompSliceLocalityRegistry *anal, CompSliceNest& n) = 0;
  virtual ~LoopBlockingAnal() {}
  virtual LoopTransformOptions::OptType GetOptimizationType()  = 0;
};

class LoopNoBlocking : public LoopBlockingAnal
{
 public:
  void SetBlocking(AstInterface& fa,  CompSliceLocalityRegistry *anal, CompSliceNest& n) ;
  virtual LoopTransformOptions::OptType GetOptimizationType() { return LoopTransformOptions::NO_OPT; }
};

class OuterLoopReuseBlocking : public LoopBlockingAnal
{
   unsigned spill;
 public:
  OuterLoopReuseBlocking(unsigned i) : spill(i) {}
  void SetBlocking(AstInterface& fa,  CompSliceLocalityRegistry *anal, CompSliceNest& n);
  virtual LoopTransformOptions::OptType GetOptimizationType() { return LoopTransformOptions::LOOP_NEST_OPT; }
};

class InnerLoopReuseBlocking : public LoopBlockingAnal
{
 public:
  void SetBlocking(AstInterface& fa,  CompSliceLocalityRegistry *anal, CompSliceNest& n);
  virtual LoopTransformOptions::OptType GetOptimizationType() { return LoopTransformOptions::LOOP_NEST_OPT; }
};

class AllLoopReuseBlocking : public LoopBlockingAnal
{
 public:
  void SetBlocking(AstInterface& fa,  CompSliceLocalityRegistry *anal, CompSliceNest& n);
  virtual LoopTransformOptions::OptType GetOptimizationType() { return LoopTransformOptions::LOOP_NEST_OPT; }
};

#endif
