#ifndef TRANSFORMATION_SEQUENCE_H
#define TRANSFORMATION_SEQUENCE_H

#include <list>
#include <utility>

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
    void setShowTransformationTrace(bool trace);
  protected:
    enum TransformationOperation { TRANSOP_ASSIGNMENT, TRANSOP_MEM_ALLOC, TRANSOP_MEM_ALLOC_ARRAY, TRANSOP_MEM_DELETE, TRANSOP_MEM_DELETE_ARRAY };
    virtual void preOrderVisit(SgNode *astNode);
    virtual void postOrderVisit(SgNode *astNode);
  private:
    typedef std::list<std::pair<TransformationOperation,SgNode*> > TransformationSequenceList;
    TransformationSequenceList transformationSequenceList;
    bool isPointerType(SgNode* node);
    bool isLocalVariable(SgExpression* exp);
    bool _showTransformationTrace;
  };
};

#endif
