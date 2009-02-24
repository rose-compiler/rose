// Author: Markus Schordan
// $Id: AstTextAttributesHandling.h,v 1.2 2006/04/24 00:21:32 dquinlan Exp $

#ifndef ASTTEXTATTRIBUTEHANDLING_H
#define ASTTEXTATTRIBUTEHANDLING_H

#include <string>
#include <sstream>
#include <iomanip>

#include "Cxx_Grammar.h"
#include "AstProcessing.h"

class AstTextAttribute : public AstAttribute {
public:
  AstTextAttribute(std::string s) : source(s) {}
  virtual std::string toString() { return source; }
 protected:
  std::string source;
};

class AstTextAttributesHandling : public SgSimpleProcessing {
public:
  AstTextAttributesHandling(SgProject* p);
  ~AstTextAttributesHandling();
  void setAll();
  void removeAll();
protected:
  enum {M_set, M_remove} mode;
  virtual void visit(SgNode* node);
  SgProject* projectnode;
};

#endif
