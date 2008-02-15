#ifndef PRE_POST_TRANSFORMATION_H
#define PRE_POST_TRANSFORMATION_H

#include <LoopTransformInterface.h>

class PrePostTransformation : public TransformAstTree
{
 protected:
  LoopTransformInterface* la;

  virtual bool operator()( AstInterface& fa, const AstNodePtr& n,
                           AstNodePtr& result) = 0;
 public:
  PrePostTransformation() : la(0) {}
  void SetLoopTransformInterface( LoopTransformInterface* _la) { la = _la; }
  AstNodePtr operator()(LoopTransformInterface& _la, const AstNodePtr& n,
                           AstInterface::TraversalVisitType t) 
    { 
       la = &_la;
       AstInterface& fa = *la;
       return TransformAstTraverse( fa, n, *this, t); 
    }
};

#endif
