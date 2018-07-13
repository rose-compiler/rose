#ifndef TYPE_TRANSFORMER_H
#define TYPE_TRANSFORMER_H

#include <string>
#include "CastTransformer.h"
#include "TFTransformation.h"
#include "ToolConfig.hpp"

class TFTypeTransformer {
 public:
  typedef std::tuple<SgType*,SgFunctionDefinition*,std::string,bool,SgType*> VarTypeVarNameTuple;
  typedef std::list<VarTypeVarNameTuple> VarTypeVarNameTupleList;
  void addToTransformationList(VarTypeVarNameTupleList& list,SgType* type, SgFunctionDefinition* funDef, std::string varnames);
  void addToTransformationList(VarTypeVarNameTupleList& list,SgType* type, SgFunctionDefinition* funDef, std::string varNames, bool base, SgType* fromType);
  // searches for variable in the given subtree 'root'
  int changeVariableType(SgNode* root, std::string varNameToFind, SgType* type);
  int changeVariableType(SgNode* root, std::string varNameToFind, SgType* type, bool base, SgType* fromType);
  int changeTypeIfInitNameMatches(SgInitializedName* varInitName, SgNode* root, std::string varNameToFind, SgType* type);
  int changeTypeIfInitNameMatches(SgInitializedName* varInitName, SgNode* root, std::string varNameToFind, SgType* type, bool base);
  int nathan_changeTypeIfFromTypeMatches(SgInitializedName* varInitName, SgNode* root, SgType* newType, SgType* fromType, bool base);
  void transformCommandLineFiles(SgProject* project);
  void transformCommandLineFiles(SgProject* project, VarTypeVarNameTupleList& list);
  SgType* nathan_rebuildBaseType(SgType* root, SgType* newBaseType);
  int nathan_changeType(SgInitializedName* varInitName, SgType* newType, SgType* oldType, std::string varName, bool base, SgFunctionDefinition* funDef);
  void makeAllCastsExplicit(SgProject* root);
  void annotateImplicitCastsAsComments(SgProject* root);
  void transformCastsInCommandLineFiles(SgProject* project);
  void setTraceFlag(bool);
  bool getTraceFlag();
  static void trace(std::string s);
  int getTotalNumChanges();
  int getTotalTypeNameChanges();
  void generateCsvTransformationStats(std::string fileName,int numTypeReplace,TFTypeTransformer& tt, TFTransformation& tfTransformation);
  void printTransformationStats(int numTypeReplace,TFTypeTransformer& tt, TFTransformation& tfTransformation);
  void nathan_addToActionList(std::string varName, std::string scope, SgType* fromType, SgType* toType, SgNode* handleNode);
  void nathan_setConfig(ToolConfig oldConfig, std::string fileName); 
 private:
  CastTransformer _castTransformer;
  static bool _traceFlag;
  int _totalNumChanges=0;
  int _totalTypeNameChanges=0;
  ToolConfig _outConfig;
  std::string _writeConfig = "";
};

#endif
