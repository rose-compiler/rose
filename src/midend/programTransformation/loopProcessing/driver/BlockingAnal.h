
#ifndef BLOCKING_ANALYSIS
#define BLOCKING_ANALYSIS

#include <CompSliceDepGraph.h>
#include <CompSliceLocality.h>
#include <LoopTransformOptions.h>
#include <vector>

class LoopBlocking 
{
  int block_index;
  int SetIndex( int index);
 protected:
  std::vector<SymbolicVal> blocksize;
  LoopBlocking() : block_index(2) {}
  virtual LoopTreeNode* 
          ApplyBlocking( const CompSliceDepGraphNode::FullNestInfo& nestInfo, 
                            LoopTreeDepComp& comp, 
                            DependenceHoisting &op, 
                            LoopTreeNode *&top);
 public:
  virtual ~LoopBlocking() {}
  virtual LoopTransformOptions::OptType GetOptimizationType()  = 0;

  /* return the innermost slice after blocking */
  virtual const CompSlice* 
  SetBlocking( CompSliceLocalityRegistry *anal, 
                        const CompSliceDepGraphNode::FullNestInfo& nestInfo)=0;

  /* QY:return the root of blocked loops (excluding block enumerating loops) */
  LoopTreeNode* apply( const CompSliceDepGraphNode::FullNestInfo& nestInfo, 
                      LoopTreeDepComp& comp, DependenceHoisting &op, 
                      LoopTreeNode *top);
  const SymbolicVal& BlockSize( int index) const { return blocksize[index]; }
  SymbolicVal& BlockSize( int index) { return blocksize[index]; }
  int NumOfLoops() const { return blocksize.size(); }
  int FirstIndex() { block_index=2; return SetIndex(blocksize.size()-1); } 
  int NextIndex(int index)
      {  if (index >= 0) return SetIndex( index - 1); 
         return -1;  }
};

class LoopNoBlocking : public LoopBlocking
{
 public:
  virtual LoopTransformOptions::OptType GetOptimizationType() { return LoopTransformOptions::NO_OPT; }
  /* return the innermost slice after blocking */
  virtual const CompSlice* 
  SetBlocking( CompSliceLocalityRegistry *anal, 
                        const CompSliceDepGraphNode::FullNestInfo& nestInfo);
};

class OuterLoopReuseBlocking : public LoopBlocking
{
  unsigned spill;
 public:
  OuterLoopReuseBlocking(unsigned i) : spill(i) {}
  virtual LoopTransformOptions::OptType GetOptimizationType() { return LoopTransformOptions::LOOP_NEST_OPT; }
  /* return the innermost slice after blocking */
  virtual const CompSlice* 
  SetBlocking( CompSliceLocalityRegistry *anal, 
                        const CompSliceDepGraphNode::FullNestInfo& nestInfo);
};

class InnerLoopReuseBlocking : public LoopBlocking
{
 public:
  virtual LoopTransformOptions::OptType GetOptimizationType() { return LoopTransformOptions::LOOP_NEST_OPT; }
  /* return the innermost slice after blocking */
  virtual const CompSlice* 
  SetBlocking( CompSliceLocalityRegistry *anal, 
                        const CompSliceDepGraphNode::FullNestInfo& nestInfo);
};

class AllLoopReuseBlocking : public LoopBlocking
{
 public:
  virtual LoopTransformOptions::OptType GetOptimizationType() { return LoopTransformOptions::LOOP_NEST_OPT; }
  /* return the innermost slice after blocking */
  virtual const CompSlice* 
  SetBlocking( CompSliceLocalityRegistry *anal, 
                        const CompSliceDepGraphNode::FullNestInfo& nestInfo);
};

class ParameterizeBlocking : public AllLoopReuseBlocking
{
 protected:
  virtual LoopTreeNode* 
          ApplyBlocking( const CompSliceDepGraphNode::FullNestInfo& nestInfo, 
                                      LoopTreeDepComp& comp, 
                                      DependenceHoisting &op, 
                                      LoopTreeNode *&top);
 public:
  virtual LoopTransformOptions::OptType GetOptimizationType() { return LoopTransformOptions::LOOP_NEST_OPT; }
  /* return the innermost slice after blocking */
  virtual const CompSlice* 
  SetBlocking( CompSliceLocalityRegistry *anal, 
                        const CompSliceDepGraphNode::FullNestInfo& nestInfo);

};

#endif
