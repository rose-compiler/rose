#ifndef TYPEFORGE_SPEC_FRONTEND_H
#define TYPEFORGE_SPEC_FRONTEND_H

#include "TFTypeTransformer.h"
#include "TFCommandList.h"

class TFSpecFrontEnd {
 public:
  // run frontend for typeforge specification file
  // returns true if reading the spec file failed
  // also reports errors
  bool parse(std::string specFileName);
  int run(SgProject* root, TFTypeTransformer& tt, TFTransformation& tfTransformation);
  int convertJSON(std::string fileName);
  int getNumTypeReplace();
  TFTypeTransformer::VarTypeVarNameTupleList getTransformationList();
 private:
  int numTypeReplace=0;
  TFTypeTransformer::VarTypeVarNameTupleList _list;
  CommandList commandList;
};

#endif
