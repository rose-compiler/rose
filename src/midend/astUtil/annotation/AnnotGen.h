#ifndef ANNOT_GEN_H
#define ANNOT_GEN_H

#include "AstInterface.h"

namespace POET {
class GenAnnot_AstParse : public TransformAstTree {
  bool operator()(AstInterface& fa, const AstNodePtr& n, AstNodePtr& result);
 public:
  void apply(AstInterface& fa, const AstNodePtr& n);
};
}

#endif
