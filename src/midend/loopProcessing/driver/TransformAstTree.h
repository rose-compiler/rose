#ifndef TRANSFORM_ASTTREE_H
#define TRANSFORM_ASTTREE_H

#include <LoopTransformInterface.h>

class BreakupStatement : public TransformAstTree
{
  int breaksize;
 public:
  BreakupStatement(int bs) : breaksize(bs) {}
  virtual bool operator()( AstInterface& fa, const AstNodePtr& n,
                           AstNodePtr& result);
};

class LoopUnrolling : public TransformAstTree
{
 public:
  typedef enum{DEFAULT = 0, COND_LEFTOVER = 1, USE_NEWVAR = 2} UnrollOpt;
 private:
  int unrollsize;
  UnrollOpt opt;
  AstNodePtr enclosingloop;
 public:
  LoopUnrolling( int sz, UnrollOpt _opt) : unrollsize(sz), opt(_opt) {}
  virtual bool operator()( AstInterface& fa, const AstNodePtr& n,
                           AstNodePtr& result);
};

#endif
