#ifndef TFTRANSFORMATION_H
#define TFTRANSFORMATION_H

class SgNode;

class TFTransformation {
 public:
  void transformHancockAccess(SgNode* root);
  int readTransformations=0;
  int writeTransformations=0;
  int statementTransformations=0;
  bool trace=false;
};


#endif
