#ifndef _SIMD_ANALYSIS_H
#define _SIMD_ANALYSIS_H

#include "rose.h"
#include "sageBuilder.h"
#include "DefUseAnalysis.h"

namespace SIMDAnalysis
{
//  Get the Def information
  void getDefList(DFAnalysis*, SgNode*);
//  Get the Use information
  void getUseList(DFAnalysis*, SgNode*);
//  Check if the loop is innermost loop
  bool isInnermostLoop(SgNode*);
//  Check if the loop has stride distance 1  
  bool isStrideOneLoop(SgNode*);

}

#endif // _SIMD_ANALYSIS_H
