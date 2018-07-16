#ifndef TYPEFORGE_SPEC_FRONTEND_H
#define TYPEFORGE_SPEC_FRONTEND_H

#include "TFTypeTransformer.h"

class TFSpecFrontEnd {
 public:
  // run frontend for typeforge specification file
  // returns true if reading the spec file failed
  // also reports errors
  bool run(std::string specFileName, SgProject* root, TFTypeTransformer& tt, TFTransformation& tfTransformation);
  int getNumTypeReplace();
  TFTypeTransformer::VarTypeVarNameTupleList getTransformationList();
 private:
  int numTypeReplace=0;
  TFTypeTransformer::VarTypeVarNameTupleList _list;
};

#endif
