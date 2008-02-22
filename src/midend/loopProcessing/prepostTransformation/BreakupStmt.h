#ifndef BREAK_UP_STMT_H
#define BREAK_UP_STMT_H

#include <PrePostTransformation.h>

class BreakupStatement : public PrePostTransformation
{
  int breaksize;
  virtual bool operator()( AstInterface& fa, const AstNodePtr& n,
                           AstNodePtr& result);
 public:
  BreakupStatement(int bs = 0) : breaksize(bs) {}
  AstNodePtr operator()( LoopTransformInterface& _la, const AstNodePtr& root) 
  {
     SetLoopTransformInterface(&_la);
     return TransformAstTraverse(_la, root, *this, AstInterface::PreVisit );
  }
  bool cmdline_configure();
  static std::string cmdline_help();
};

#endif
