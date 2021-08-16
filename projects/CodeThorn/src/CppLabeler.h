
#ifndef CPP_LABELER_H
#define CPP_LABELER_H 1

#include "sage3basic.h"
#include "Labeler.h"

namespace CodeThorn
{

struct CppLabeler : CLabeler
{
    using base = CLabeler;
    using base::base;

  protected:
    bool isFunctionCallNode(SgNode*) const ROSE_OVERRIDE;
};

} // CodeThorn
#endif /* CPP_LABELER_H */
