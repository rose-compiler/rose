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
};

class ParallelizeBlocking : public LoopPar
{
  virtual LoopTreeNode* apply( const CompSliceDepGraphNode::FullNestInfo& nestInfo,
                                      LoopTreeDepComp& comp, 
                                      DependenceHoisting &op, 
                                      LoopTreeNode *&top);
  virtual LoopTransformOptions::OptType GetOptimizationType() { return LoopTransformOptions::PAR_LOOP_OPT; }
};

#endif
