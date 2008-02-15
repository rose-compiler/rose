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
  AstNodePtr operator()( LoopTransformInterface& la, const AstNodePtr& root) 
  {
     SetLoopTransformInterface(&la);
     return TransformAstTraverse(la, root, *this, AstInterface::PreVisit );
  }
//Boolean cmdline_configure();
  int cmdline_configure();
  static std::string cmdline_help();
};

#endif
