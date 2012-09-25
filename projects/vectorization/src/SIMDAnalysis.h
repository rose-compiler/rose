#ifndef _SIMD_ANALYSIS_H
#define _SIMD_ANALYSIS_H

#include "rose.h"
#include "sageBuilder.h"
#include "DefUseAnalysis.h"

namespace SIMDAnalysis
{
//  Check if the loop is innermost loop
  bool isInnermostLoop(SgNode*);
//  Check if the loop has stride distance 1  
  bool isStrideOneLoop(SgNode*);

  std::vector<SgInitializedName*> getLoopIndexVariable(SgNode*);
  bool isLoopIndexVariable(SgInitializedName*, SgNode*);
}

#endif // _SIMD_ANALYSIS_H
