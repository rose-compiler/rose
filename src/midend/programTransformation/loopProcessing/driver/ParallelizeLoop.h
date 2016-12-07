#ifndef PARALLELIZE_LOOP_H
#define PARALLELIZE_LOOP_H

#include <LoopTransformOptions.h>
#include <CompSliceDepGraph.h>
#include <CompSliceLocality.h>

class LoopPar
{
 public:
  virtual LoopTransformOptions::OptType GetOptimizationType() =0;
  virtual LoopTreeNode* apply( const CompSliceDepGraphNode::FullNestInfo& nestInfo,
                                      LoopTreeDepComp& comp, 
                                      DependenceHoisting &op, 
                                      LoopTreeNode *&top)=0;

// DQ (12/6/2016): Added explicit virtual destructor to eliminate warning we want to be an error: -Wdelete-non-virtual-dtor.
   virtual ~LoopPar() {}
};

class ParallelizeBlocking : public LoopPar
{
  virtual LoopTreeNode* apply( const CompSliceDepGraphNode::FullNestInfo& nestInfo,
                                      LoopTreeDepComp& comp, 
                                      DependenceHoisting &op, 
                                      LoopTreeNode *&top);
  virtual LoopTransformOptions::OptType GetOptimizationType() { return LoopTransformOptions::PAR_LOOP_OPT; }


// DQ (12/6/2016): Added explicit virtual destructor to eliminate warning we want to be an error: -Wdelete-non-virtual-dtor.
  public:
       virtual ~ParallelizeBlocking() {}
};

#endif
