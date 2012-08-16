#ifndef _SIMD_ANALYSIS_H
#define _SIMD_ANALYSIS_H

#include "rose.h"
#include "sageBuilder.h"
#include "DefUseAnalysis.h"

namespace SIMDAnalysis
{

  void getDefList(DFAnalysis*, SgNode*);
  void getUseList(DFAnalysis*, SgNode*);
  bool isInnermostLoop(SgForStatement*);
  bool isStrideOneLoop(SgNode*);

}

#endif // _SIMD_ANALYSIS_H
