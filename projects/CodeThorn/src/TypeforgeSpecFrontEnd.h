#ifndef TYPEFORGE_SPEC_FRONTEND_H
#define TYPEFORGE_SPEC_FRONTEND_H

#include "TypeTransformer.h"

class TypeforgeSpecFrontEnd {
 public:
  // run frontend for typeforge specification file
  // returns true if reading the spec file failed
  // also reports errors
  bool run(std::string specFileName, SgProject* root, TypeTransformer& tt, TFTransformation& tfTransformation);
  int getNumTypeReplace();
  TypeTransformer::VarTypeVarNameTupleList getTransformationList();
 private:
  int numTypeReplace=0;
  TypeTransformer::VarTypeVarNameTupleList _list;
};

#endif
