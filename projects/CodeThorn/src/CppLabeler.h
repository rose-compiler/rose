
#ifndef CPP_LABELER_H
#define CPP_LABELER_H 1

#include "sage3basic.h"
#include "Labeler.h"

namespace CodeThorn
{

struct CppLabeler : CLabeler
{
  CppLabeler();
  explicit CppLabeler(SgNode*);

  // was: isFunctionCallNode(SgNode* n) const ROSE_OVERRIDE;
};

} // CodeThorn
#endif /* CPP_LABELER_H */
