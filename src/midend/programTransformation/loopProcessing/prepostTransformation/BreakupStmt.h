#ifndef BREAK_UP_STMT_H
#define BREAK_UP_STMT_H

#include <PrePostTransformation.h>

class BreakupStatement : public PrePostTransformation
{
  static size_t breaksize;
  virtual bool operator()( AstInterface& fa, const AstNodePtr& n,
                           AstNodePtr& result);
 public:
  AstNodePtr operator()(const AstNodePtr& root) 
  { return PrePostTransformation::operator()(root, AstInterface::PreVisit); }
  static size_t get_breaksize() { return breaksize; }
  static void cmdline_configure(const std::vector<std::string>& argv,
                                std::vector<std::string>* unknown_args=0); 
  static std::string cmdline_help();
};

#endif
