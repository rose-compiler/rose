#ifndef TRANSFORMATION_SEQUENCE_H
#define TRANSFORMATION_SEQUENCE_H

#include <list>

#include "sage3basic.h"

class SgNode;
class SgExpression;

namespace Backstroke {
  class TransformationSequence : protected AstPrePostProcessing {
  public:
    TransformationSequence();
    virtual ~TransformationSequence();
    void reset();
    void create(SgNode*);
    void apply();
  protected:
    virtual void preOrderVisit(SgNode *astNode);
    virtual void postOrderVisit(SgNode *astNode);
  private:
    std::list<SgNode*> transformationSequence;
    std::list<SgNode*> transformationSequenceCommit;
    bool isPointerType(SgNode* node);
    bool isLocalVariable(SgExpression* exp);
  };
};

#endif
