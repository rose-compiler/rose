#ifndef _NORMALIZATION_H
#define _NORMALIZATION_H

#include "rose.h"
#include "sageBuilder.h"

namespace normalization
{
  void swapOperands(SgBinaryOp*);
  void normalizeExperission(SgProject*);
}

#endif // _NORMALIZATION_H
