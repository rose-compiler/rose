
#ifndef CPP_LABELER_H
#define CPP_LABELER_H 1

#include "sage3basic.h"
#include "Labeler.h"

namespace CodeThorn
{

// \deprecated currently not used
class CppLabeler : CLabeler
{
  CppLabeler();
  //~ explicit CppLabeler(SgNode*);

  // was: isFunctionCallNode(SgNode* n) const override;
};

} // CodeThorn
#endif /* CPP_LABELER_H */
