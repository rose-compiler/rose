#include <string>
#include "CastTransformer.h"

class TypeTransformer {
 public:
  typedef std::pair<SgType*,std::string> VarTypeVarNamePair;
  typedef std::list<VarTypeVarNamePair> VarTypeVarNamePairList;
  void addToTransformationList(VarTypeVarNamePairList& list,SgType* type, std::string varNames);
  void changeVariableType(SgProject* root, std::string varNameToFind, SgType* type);
  void transformCommandLineFiles(SgProject* project);
  void transformCommandLineFiles(SgProject* project, VarTypeVarNamePairList& list);
  void makeAllCastsExplicit(SgProject* root);
  void annotateImplicitCastsAsComments(SgProject* root);
  void transformCastsInCommandLineFiles(SgProject* project);
 private:
  CastTransformer _castTransformer;
};
