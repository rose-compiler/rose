#include <ParallelizeLoop.h>
#include <AutoTuningInterface.h>
#include "RoseAsserts.h" /* JFR: Added 17Jun2020 */

LoopTreeNode* ParallelizeBlocking::
apply( const CompSliceDepGraphNode::FullNestInfo& nestInfo,
              LoopTreeDepComp& comp, DependenceHoisting &op,
              LoopTreeNode *&top)
{
  AutoTuningInterface* tuning = LoopTransformInterface::getAutoTuningInterface();
  assert(tuning != 0);

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
  tuning->ParallelizeLoop(top, bsize);
  return top;
}
