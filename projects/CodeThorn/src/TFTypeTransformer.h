#ifndef TYPE_TRANSFORMER_H
#define TYPE_TRANSFORMER_H

#include <string>
#include "CastTransformer.h"
#include "TFTransformation.h"

class TFTypeTransformer {
 public:
  typedef std::tuple<SgType*,SgFunctionDefinition*,std::string,bool,SgType*> VarTypeVarNameTuple;
  typedef std::list<VarTypeVarNameTuple> VarTypeVarNameTupleList;
  void addToTransformationList(VarTypeVarNameTupleList& list,SgType* type, SgFunctionDefinition* funDef, std::string varNames, bool base=false, SgType* fromType = nullptr);
  // searches for variable in the given subtree 'root'
  int changeVariableType(SgNode* root, std::string varNameToFind, SgType* type, bool base=false, SgType* fromType=nullptr);
  int changeTypeIfInitNameMatches(SgInitializedName* varInitName, SgNode* root, std::string varNameToFind, SgType* type, bool base=false);
  void transformCommandLineFiles(SgProject* project);
  void transformCommandLineFiles(SgProject* project, VarTypeVarNameTupleList& list);
  void makeAllCastsExplicit(SgProject* root);
  void annotateImplicitCastsAsComments(SgProject* root);
  void transformCastsInCommandLineFiles(SgProject* project);
  void setTraceFlag(bool);
  bool getTraceFlag();
  static void trace(std::string s);
  int getTotalNumChanges();
  void generateCsvTransformationStats(std::string fileName,int numTypeReplace,TFTypeTransformer& tt, TFTransformation& tfTransformation);
  void printTransformationStats(int numTypeReplace,TFTypeTransformer& tt, TFTransformation& tfTransformation);

 private:
  CastTransformer _castTransformer;
  static bool _traceFlag;
  int _totalNumChanges=0;
};

#endif
