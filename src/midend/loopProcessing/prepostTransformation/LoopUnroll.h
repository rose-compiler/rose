#ifndef LOOP_UNROLL_H
#define LOOP_UNROLL_H

#include <PrePostTransformation.h>
#include <SinglyLinkedList.h>

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
  AstNodePtr operator()( LoopTransformInterface& _la, const AstNodePtr& root) 
  {
     SetLoopTransformInterface(&_la);
     return TransformAstTraverse(_la, root, *this, AstInterface::PostVisit );
  }
  bool cmdline_configure(); 
  static STD string cmdline_help() ;
};

#endif
