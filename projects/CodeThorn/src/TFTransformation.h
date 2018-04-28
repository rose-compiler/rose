#ifndef TFTRANSFORMATION_H
#define TFTRANSFORMATION_H

#include <string>

class SgNode;

class TFTransformation {
 public:
  void transformHancockAccess(std::string exprTypeName, SgNode* root);
  int readTransformations=0;
  int writeTransformations=0;
  int statementTransformations=0;
  bool trace=false;
  void transformRhs(std::string exprTypeName, SgNode* rhsRoot);
  void checkAndTransformVarAssignments(std::string exprTypeName,SgNode* root);
};


#endif
