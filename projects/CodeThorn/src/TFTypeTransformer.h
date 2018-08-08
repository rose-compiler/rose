#ifndef TYPE_TRANSFORMER_H
#define TYPE_TRANSFORMER_H

#include <string>
#include "CastTransformer.h"
#include "TFTransformation.h"
#include "ToolConfig.hpp"

class TFTypeTransformer;

class TypeTransformer{
  private:
    typedef std::tuple<std::string,SgType*> ReplacementTuple;
    std::map<SgNode*, ReplacementTuple> transformations;
    int transformationsCount = 0;
  public:
    int transform();
    int addTransformation(std::string key, SgType* newType, SgNode* node);
};

class TransformDirective{
  protected:
    bool base;
    bool listing;
    SgType* toType;
    TransformDirective(bool transformBase, bool onlyList, SgType* to_type);
  public:
    virtual int run(SgProject* project, TFTypeTransformer* tt) = 0;
};

class NameTransformDirective : public TransformDirective{
  private:
    SgFunctionDefinition* funDef;
    std::string name;
  public:
    NameTransformDirective(std::string varName, SgFunctionDefinition* functionDefinition, bool base, bool listing, SgType* toType);
    int run(SgProject* project, TFTypeTransformer* tt);
};

class TypeTransformDirective : public TransformDirective{
  private:
    SgFunctionDefinition* funDef;
    SgType* fromType;
    std::string location;
  public:
    TypeTransformDirective(std::string functionLocation, SgFunctionDefinition* functionDefinition, SgType* from_type, bool base, bool listing, SgType* toType);
    int run(SgProject* project, TFTypeTransformer* tt);
};

class HandleTransformDirective : public TransformDirective{
  private:
    SgNode* node;
  public:
    HandleTransformDirective(SgNode* handleNode, bool base, bool listing, SgType* toType);
    int run(SgProject* project, TFTypeTransformer* tt);
};


class TFTypeTransformer {
 public:
  typedef TransformDirective* VarTypeVarNameTuple;
  typedef std::list<VarTypeVarNameTuple> VarTypeVarNameTupleList;
  //Methods to add transformation directives
  void addHandleTransformationToList(VarTypeVarNameTupleList& list,SgType* type,bool base,SgNode* handleNode, bool listing);
  void addTypeTransformationToList(VarTypeVarNameTupleList& list,SgType* type, SgFunctionDefinition* funDef, std::string varNames, bool base, SgType* fromType, bool listing);
  void addNameTransformationToList(VarTypeVarNameTupleList& list,SgType* type, SgFunctionDefinition* funDef, std::string varNames, bool base=false, bool listing=false);
  // searches for variable in the given subtree 'root'
  int changeVariableType(SgNode* root, std::string varNameToFind, SgType* type);
  int changeVariableType(SgNode* root, std::string varNameToFind, SgType* type, bool base, SgType* fromType, bool listing);
  int changeTypeIfInitNameMatches(SgInitializedName* varInitName, SgNode* root, std::string varNameToFind, SgType* type);
  int changeTypeIfInitNameMatches(SgInitializedName* varInitName, SgNode* root, std::string varNameToFind, SgType* type, bool base, SgNode* handleNode, bool listing);
  int nathan_changeTypeIfFromTypeMatches(SgInitializedName* varInitName, SgNode* root, SgType* newType, SgType* fromType, bool base, SgNode* handleNode, bool listing);
  void transformCommandLineFiles(SgProject* project);
  void transformCommandLineFiles(SgProject* project, VarTypeVarNameTupleList& list);
  void analyzeTransformations(SgProject* project, VarTypeVarNameTupleList& list);
  void executeTransformations(SgProject* project);
  SgType* nathan_rebuildBaseType(SgType* root, SgType* newBaseType);
  int nathan_changeType(SgInitializedName* varInitName, SgType* newType, SgType* oldType, std::string varName, bool base, SgFunctionDefinition* funDef, SgNode* handleNode,bool listing);
  void makeAllCastsExplicit(SgProject* root);
  void annotateImplicitCastsAsComments(SgProject* root);
  void transformCastsInCommandLineFiles(SgProject* project);
  void setTraceFlag(bool);
  bool getTraceFlag();
  static void trace(std::string s);
  int getTotalNumChanges();
  void generateCsvTransformationStats(std::string fileName,int numTypeReplace,TFTypeTransformer& tt, TFTransformation& tfTransformation);
  void printTransformationStats(int numTypeReplace,TFTypeTransformer& tt, TFTransformation& tfTransformation);
  void nathan_addToActionList(std::string varName, std::string scope, SgType* fromType, SgType* toType, SgNode* handleNode, bool base);
  void nathan_setConfig(ToolConfig* config);
  void nathan_setConfigFile(std::string fileName);
  int  nathan_changeHandleType(SgNode* handle, SgType* newType, bool base, bool listing);
 private:
  CastTransformer _castTransformer;
  TypeTransformer _typeTransformer;
  static bool _traceFlag;
  int _totalNumChanges=0;
  ToolConfig* _outConfig;
  std::string _writeConfig = "";
};

#endif
