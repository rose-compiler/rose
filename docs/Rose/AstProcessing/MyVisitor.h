#include "rose.h"

class MyVisitor : public AstSimpleProcessing {
 protected:
  void virtual visit(SgNode* astNode);
}

