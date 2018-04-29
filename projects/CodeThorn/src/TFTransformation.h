#ifndef TFTRANSFORMATION_H
#define TFTRANSFORMATION_H

#include <string>

class SgNode;

class TFTransformation {
 public:
  void transformHancockAccess(SgType* exprTypeName, SgNode* root);
  int readTransformations=0;
  int writeTransformations=0;
  int statementTransformations=0;
  bool trace=false;
  void transformRhs(SgType* accessType, SgNode* rhsRoot);
  void checkAndTransformVarAssignments(SgType* accessType,SgNode* root);
  void checkAndTransformNonAssignments(SgType* accessType,SgNode* root);
};


#endif
