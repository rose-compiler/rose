#ifndef TYPE_TRANSFORMER_H
#define TYPE_TRANSFORMER_H

#include <string>
#include "CastTransformer.h"
#include "TFTransformation.h"
#include "ToolConfig.hpp"
#include "Analysis.h"

namespace Typeforge {

class TFTypeTransformer;

class Transformer{
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
    SgFunctionDeclaration* funDecl;
    std::string name;
  public:
    NameTransformDirective(std::string varName, SgFunctionDeclaration* functionDeclaration, bool base, bool listing, SgType* toType);
    int run(SgProject* project, TFTypeTransformer* tt);
};

class TypeTransformDirective : public TransformDirective{
  private:
    SgFunctionDeclaration* funDecl;
    SgType* fromType;
    std::string location;
  public:
    TypeTransformDirective(std::string functionLocation, SgFunctionDeclaration* functionDeclaration, SgType* from_type, bool base, bool listing, SgType* toType);
    int run(SgProject* project, TFTypeTransformer* tt);
};

class HandleTransformDirective : public TransformDirective{
  private:
    SgNode* node;
  public:
    HandleTransformDirective(SgNode* handleNode, bool base, bool listing, SgType* toType);
    int run(SgProject* project, TFTypeTransformer* tt);
};

class FileTransformDirective : public TransformDirective{
  private:
    std::string fileName;
  public:
    FileTransformDirective(std::string file);
    int run(SgProject* project, TFTypeTransformer* tt);
};

class SetTransformDirective : public TransformDirective{
  private:
    bool flag;
  public:
    SetTransformDirective(bool value);
    int run(SgProject* project, TFTypeTransformer* tt);
};

class TFTypeTransformer {
 public:
  typedef TransformDirective* VarTypeVarNameTuple;
  typedef std::list<VarTypeVarNameTuple> VarTypeVarNameTupleList;
  //Methods to add transformation directives
  void addHandleTransformationToList(VarTypeVarNameTupleList& list,SgType* type,bool base,SgNode* handleNode, bool listing);
  void addTypeTransformationToList(VarTypeVarNameTupleList& list,SgType* type, SgFunctionDeclaration* funDecl, std::string varNames, bool base, SgType* fromType, bool listing);
  void addNameTransformationToList(VarTypeVarNameTupleList& list,SgType* type, SgFunctionDeclaration* funDecl, std::string varNames, bool base=false, bool listing=false);
  void addFileChangeToList(VarTypeVarNameTupleList& list, std::string file);
  void addSetChangeToList(VarTypeVarNameTupleList& list, bool flag);

  // searches for variable in the given subtree 'root'
  int changeVariableType(SgProject * project, SgFunctionDeclaration* funDecl, std::string varNameToFind, SgType* type, bool base=false, SgType* fromType=nullptr, bool listing=false);

  int changeTypeIfInitNameMatches(SgInitializedName* varInitName, SgNode* root, std::string varNameToFind, SgType* type, bool base=false, SgNode* handleNode=nullptr, bool listing=false);
  int changeTypeIfFromTypeMatches(SgInitializedName* varInitName, SgNode* root, SgType* newType, SgType* fromType, bool base, SgNode* handleNode, bool listing);

  void transformCommandLineFiles(SgProject* project);
  void transformCommandLineFiles(SgProject* project, VarTypeVarNameTupleList& list);
  void analyzeTransformations(SgProject* project, VarTypeVarNameTupleList& list);
  void executeTransformations(SgProject* project);
  SgType* rebuildBaseType(SgType* root, SgType* newBaseType);
  int changeType(SgInitializedName* varInitName, SgType* newType, SgType* oldType, std::string varName, bool base, SgFunctionDeclaration* funDecl, SgNode* handleNode,bool listing);
  void makeAllCastsExplicit(SgProject* root);
  void annotateImplicitCastsAsComments(SgProject* root);
  void transformCastsInCommandLineFiles(SgProject* project);
  void setTraceFlag(bool);
  bool getTraceFlag();
  static void trace(std::string s);
  int getTotalNumChanges();
  void generateCsvTransformationStats(std::string fileName,int numTypeReplace,TFTypeTransformer& tt, TFTransformation& tfTransformation);
  void printTransformationStats(int numTypeReplace,TFTypeTransformer& tt, TFTransformation& tfTransformation);
  void addToActionList(std::string varName, SgNode * scope, SgType* fromType, SgType* toType, SgNode* handleNode, bool base);
  void setConfigFile(std::string fileName);
  void writeConfig();
  int  changeHandleType(SgNode* handle, SgType* newType, bool base, bool listing);
  bool changeSetFlag(bool value);
  std::set<SgNode*>* getSet(SgNode* node, SgType* type);
  void writeSets(SgProject* project, SgType* type, std::string toTypeString);
  int changeSet(SgNode* node, SgType* fromType, SgType* toType, bool base, bool listing);
 private:
  CastTransformer _castTransformer;
  Transformer _transformer;
  std::map<SgType*, Analysis*> typeSets;
  static bool _traceFlag;
  bool _setFlag = false;
  int _totalNumChanges=0;
  ToolConfig* _outConfig = nullptr;
  std::string _writeConfig = "";
};

}

#endif
