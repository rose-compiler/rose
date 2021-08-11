
#include "CppLabeler.h"
#include "SgNodeHelper.h"

namespace CodeThorn
{

bool CppLabeler::isFunctionCallNode(SgNode* n) const
{
  return SgNodeHelper::matchExtendedNormalizedCall(n, true /* use matcher */);
}

}
