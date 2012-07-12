#ifndef LOOP_UNROLL_H
#define LOOP_UNROLL_H

#include <PrePostTransformation.h>
#include <SinglyLinkedList.h>

class LoopUnrolling : public PrePostTransformation
{
 public:
  typedef enum{DEFAULT = 0, COND_LEFTOVER = 1, USE_NEWVAR = 2, POET_TUNING=4} UnrollOpt;
 private:
  static unsigned unrollsize;
  static UnrollOpt opt;
  AstNodePtr enclosingloop;

  virtual bool operator()( AstInterface& fa, const AstNodePtr& n,
                           AstNodePtr& result);
 public:
  AstNodePtr operator()(const AstNodePtr& root) 
  {
     AstInterface& fa = LoopTransformInterface::getAstInterface();
     return TransformAstTraverse(fa, root, *this, AstInterface::PostVisit );
  }
  static unsigned get_unrollsize() { return unrollsize; }
  static void cmdline_configure(const std::vector<std::string>& argv,
                                std::vector<std::string>* unknown_args=0); 
  static std::string cmdline_help() ;
};

#endif
