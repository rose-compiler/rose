#ifndef AST_TERM_REPRESENTATION
#define AST_TERM_REPRESENTATION

#include <string>
#include "AstTerm.h"
#include "DFAstAttribute.h"

class AstTermRepresentationAttribute : public SPRAY::DFAstAttribute {
 public:
  AstTermRepresentationAttribute(SgNode* node);
  std::string toString();
  static void attachAstTermRepresentationAttributes(SgNode* node);
 private:
  SgNode* _node;
};

#endif
