#include <ParallelizeLoop.h>
#include <AutoTuningInterface.h>

LoopTreeNode* ParallelizeBlocking::
apply( const CompSliceDepGraphNode::FullNestInfo& nestInfo, 
              LoopTreeDepComp& comp, DependenceHoisting &op, 
              LoopTreeNode *&top)
{
// DQ (1/14/2017): make dependence on POET optional.
#ifdef ROSE_USE_POET
  AutoTuningInterface* tuning = LoopTransformInterface::getAutoTuningInterface();
  assert(tuning != 0);
#endif

  const CompSliceNest* pslices = nestInfo.GetNest();
  assert(pslices != 0);
  const CompSliceNest& slices = *pslices;
  const CompSlice* outer = slices[0];
  for (CompSlice::ConstLoopIterator ploops = outer->GetConstLoopIterator();
       !ploops.ReachEnd(); ++ploops) {
     CompSlice::SliceLoopInfo cur = ploops.CurrentInfo();
     if (!cur.reversible) { //* QY: not parallelizable; return
        return top;
     }
  } 
  /*QY: parallelize the outermost loop*/
  top = op.Transform( comp, outer, top);
  int bsize = LoopTransformOptions::GetInstance()->GetParBlockSize();

// DQ (1/14/2017): make dependence on POET optional.
#ifdef ROSE_USE_POET
  tuning->ParallelizeLoop(top, bsize);  
#endif

  return top;
}
