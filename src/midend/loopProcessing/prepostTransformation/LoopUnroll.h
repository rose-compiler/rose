#ifndef LOOP_UNROLL_H
#define LOOP_UNROLL_H

#include <PrePostTransformation.h>

class LoopUnrolling : public PrePostTransformation
{
 public:
  typedef enum{DEFAULT = 0, COND_LEFTOVER = 1, USE_NEWVAR = 2} UnrollOpt;
 private:
  int unrollsize;
  UnrollOpt opt;
  AstNodePtr enclosingloop;

  virtual bool operator()( AstInterface& fa, const AstNodePtr& n,
                           AstNodePtr& result);
 public:
  LoopUnrolling( int sz = 0, UnrollOpt _opt = DEFAULT) : unrollsize(sz), opt(_opt) {}
  AstNodePtr operator()( LoopTransformInterface& la, const AstNodePtr& root) 
  {
     SetLoopTransformInterface(&la);
     return TransformAstTraverse(la, root, *this, AstInterface::PostVisit );
  }
//Boolean cmdline_configure(); 
  int cmdline_configure(); 
  static std::string cmdline_help() ;
};

#endif
