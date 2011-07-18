#ifndef PRIVATIZE_SCALAR_H
#define PRIVATIZE_SCALAR_H

#include <PrePostTransformation.h>

class PrivatizeScalar
{
 public:
  AstNodePtr operator()( const AstNodePtr& root); 
  bool cmdline_configure();
  static std::string cmdline_help();
};

#endif
