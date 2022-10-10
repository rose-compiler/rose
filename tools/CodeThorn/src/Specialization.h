#ifndef SPECIALIZATION_H
#define SPECIALIZATION_H

#include "VariableIdMapping.h"
#include "EState.h"
#include "ArrayElementAccessData.h"
#include "CTAnalysis.h"
#include "EStateTransferFunctions.h"
#include "RewriteSystem.h"
#include "ReadWriteData.h"
#include "Visualizer.h"
#include "LoopInfo.h"
#include "CollectionOperators.h"

#include <iostream>

typedef std::vector< std::pair< CodeThorn::VariableId, IterVarType> > IterationVariables;

struct EStateExprInfo {
  CodeThorn::EStatePtr first;
  SgExpression* originalExpr;
  SgExpression* second;
  bool mark;
  SgForStatement* forLoop;
EStateExprInfo():first(0),second(0),mark(false){}
EStateExprInfo(CodeThorn::EStatePtr estate,SgExpression* originalExpr, SgExpression* transformedExp):first(estate),originalExpr(originalExpr),second(transformedExp),mark(false),forLoop(0){
}
};

typedef std::vector<EStateExprInfo> ArrayUpdatesSequence;

enum SAR_MODE { SAR_SUBSTITUTE, SAR_SSA };

class NumberAstAttribute : public AstAttribute {
public:
  int index;
  NumberAstAttribute():index(-1){}
  NumberAstAttribute(int index):index(index){}
  std::string toString() {
    std::stringstream ss;
    ss<<index;
    return ss.str();
  }
  virtual AstAttribute::OwnershipPolicy
    getOwnershipPolicy() const override {
    return CONTAINER_OWNERSHIP;
  }
};

class ConstReporter {
 public:
  virtual ~ConstReporter();
  virtual bool isConst(SgNode* node)=0;
  virtual int getConstInt()=0;
  virtual CodeThorn::VariableId getVariableId()=0;
  virtual SgVarRefExp* getVarRefExp()=0;
};

class PStateConstReporter : public ConstReporter {
public:
  PStateConstReporter(CodeThorn::PStatePtr pstate, CodeThorn::VariableIdMapping* variableIdMapping);
  bool isConst(SgNode* node);
  int getConstInt();
  CodeThorn::VariableId getVariableId();
  SgVarRefExp* getVarRefExp();
private:
  CodeThorn::PStatePtr _pstate;
  CodeThorn::VariableIdMapping* _variableIdMapping;
  SgVarRefExp* _varRefExp;
};

class SpecializationConstReporter : public ConstReporter {
public:
  SpecializationConstReporter(CodeThorn::VariableIdMapping* variableIdMapping, CodeThorn::VariableId var, int constInt);
  virtual bool isConst(SgNode* node);
  virtual int getConstInt();
  virtual CodeThorn::VariableId getVariableId();
  virtual SgVarRefExp* getVarRefExp();
private:
  CodeThorn::VariableIdMapping* _variableIdMapping;
  CodeThorn::VariableId _variableId;
  SgVarRefExp* _varRefExp;
  int _constInt;
};
  
class Specialization {
 public:
  Specialization();

  static void initDiagnostics();
 
  void transformArrayProgram(SgProject* root, CTAnalysis* analyzer);
  void extractArrayUpdateOperations(CTAnalysis* ana,
                                    ArrayUpdatesSequence& arrayUpdates,
                                    RewriteSystem& rewriteSystem,
                                    bool useConstExprSubstRule=true
                                    );
  void printUpdateInfos(ArrayUpdatesSequence& arrayUpdates, CodeThorn::VariableIdMapping* variableIdMapping);
  void writeArrayUpdatesToFile(ArrayUpdatesSequence& arrayUpdates, std::string filename, SAR_MODE sarMode, bool performSorting);
  void createSsaNumbering(ArrayUpdatesSequence& arrayUpdates, CodeThorn::VariableIdMapping* variableIdMapping);
  // specializes function with name funNameToFind and replace variable of parameter param with constInt
  int specializeFunction(SgProject* project, std::string funNameToFind, int param, int constInt, CodeThorn::VariableIdMapping* variableIdMapping);
  int specializeFunction(SgProject* project, std::string funNameToFind, int param, int constInt, std::string varInitName, int initConst, CodeThorn::VariableIdMapping* variableIdMapping);
  SgFunctionDefinition* getSpecializedFunctionRootNode() { return _specializedFunctionRootNode; }
  void substituteArrayRefs(ArrayUpdatesSequence& arrayUpdates, CodeThorn::VariableIdMapping* variableIdMapping, SAR_MODE sarMode, RewriteSystem& rewriteSystem);
  void setMaxNumberOfExtractedUpdates(long);
  long getMaxNumberOfExtractedUpdates();
  bool dataRaceDetection=false; // temporary flag
 private:
  static Sawyer::Message::Facility logger;
  std::string iterVarsToString(IterationVariables iterationVars, CodeThorn::VariableIdMapping* variableIdMapping);
  int substituteConstArrayIndexExprsWithConst(CodeThorn::VariableIdMapping* variableIdMapping, CodeThorn::EStateTransferFunctions* exprAnalyzer, CodeThorn::EStatePtr estate, SgNode* root);
  CodeThorn::VariableId determineVariableIdToSpecialize(SgFunctionDefinition* funDef, int param, CodeThorn::VariableIdMapping* variableIdMapping);

  // replaces each use of SgVarRefExp if the corresponding variableId in pstate is constant (with this constant)
  int substituteVariablesWithConst(CodeThorn::VariableIdMapping* variableIdMapping, CodeThorn::PStatePtr pstate, SgNode *node);

  // replaces each use of a SgVarRefExp of variableId with constInt.
  int substituteVariablesWithConst(SgNode* node, CodeThorn::VariableIdMapping* variableIdMapping, CodeThorn::VariableId variableId, int constInt);
  int substituteVarInitWithConst(SgFunctionDefinition* funDef, CodeThorn::VariableIdMapping* variableIdMapping, std::string varInitName, int varInitConstInt);

  // replace each use of a SgVarRefExp according to constReporter
  int substituteVariablesWithConst(SgNode* node, ConstReporter* constReporter);

  SgNode* findDefAssignOfArrayElementUse(SgPntrArrRefExp* useRefNode, ArrayUpdatesSequence& arrayUpdates, ArrayUpdatesSequence::iterator pos, CodeThorn::VariableIdMapping* variableIdMapping);
  SgNode* findDefAssignOfUse(SgVarRefExp* useRefNode, ArrayUpdatesSequence& arrayUpdates, ArrayUpdatesSequence::iterator pos, CodeThorn::VariableIdMapping* variableIdMapping);
  void attachSsaNumberingtoDefs(ArrayUpdatesSequence& arrayUpdates, CodeThorn::VariableIdMapping* variableIdMapping);
  std::string flattenArrayInitializer(SgVariableDeclaration* decl, CodeThorn::VariableIdMapping* variableIdMapping);
  void transformArrayAccess(SgNode* node, CodeThorn::VariableIdMapping* variableIdMapping);
  
  SgFunctionDefinition* _specializedFunctionRootNode=0;
  long _maxNumberOfExtractedUpdates=-1;
};


#endif
