#include "MatlabToCpp.h"

#include "rose.h"

#include "MatlabSimpleTransformer.h"
#include "ExpressionTransformer.h"
#include "MatrixTypeTransformer.h"
#include "NarginTransformer.h"

namespace MatlabToCpp
{
  void transform(SgProject* proj)
  {
    ROSE_ASSERT(proj);

    transformReturnListAttribute(proj);
    transformNargin(proj);
    transformMatrixOnAssignOp(proj);
    transformReturnStatement(proj);
    transformMatrixOnFunctionCallArguments(proj);
    transformRangeExpression(proj);
    transformForloop(proj);
    transformSelectedCalls(proj);

    // expression transformations
    transformLeftDivide(proj);
    transformRightDivide(proj);
    transformPowerOp(proj);
    transformTranspose(proj);
    transformElementwiseMultiply(proj);
    transformElementwiseRightDivide(proj);
    transformMatrixType(proj);
  }
}
