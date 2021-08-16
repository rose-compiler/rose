
#include "CppLabeler.h"
#include "SgNodeHelper.h"

namespace CodeThorn
{

namespace
{
  bool matchCxxCall(SgNode* n)
  {
    return SgNodeHelper::matchExtendedNormalizedCall(n);
  }
};

CppLabeler::CppLabeler()
: CLabeler()
{
  setIsFunctionCallFn(matchCxxCall);
}

CppLabeler::CppLabeler(SgNode* n)
: CLabeler(n)
{
  setIsFunctionCallFn(matchCxxCall);
}

#if 0
bool CppLabeler::isFunctionCallNode(SgNode* n) const
{
  return matchCxxCall(n);
}
#endif
}
