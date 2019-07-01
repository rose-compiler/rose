#ifndef SPECIALIZATION_H
#define SPECIALIZATION_H

#include "VariableIdMapping.h"
#include "EState.h"
#include "ArrayElementAccessData.h"
#include "Analyzer.h"
#include "ExprAnalyzer.h"
#include "RewriteSystem.h"
#include "ReadWriteData.h"
#include "Visualizer.h"
#include "LoopInfo.h"
#include "CollectionOperators.h"

// BOOST includes
#include "boost/lexical_cast.hpp"

#include <iostream>

using namespace std;
using namespace CodeThorn;
using namespace CodeThorn;

typedef vector< pair< VariableId, IterVarType> > IterationVariables;

struct EStateExprInfo {
  const EState* first;
  SgExpression* originalExpr;
  SgExpression* second;
  bool mark;
  SgForStatement* forLoop;
EStateExprInfo():first(0),second(0),mark(false){}
EStateExprInfo(const EState* estate,SgExpression* originalExpr, SgExpression* transformedExp):first(estate),originalExpr(originalExpr),second(transformedExp),mark(false),forLoop(0){
}
};

typedef vector<EStateExprInfo> ArrayUpdatesSequence;

enum SAR_MODE { SAR_SUBSTITUTE, SAR_SSA };

class NumberAstAttribute : public AstAttribute {
public:
  int index;
  NumberAstAttribute():index(-1){}
  NumberAstAttribute(int index):index(index){}
  string toString() {
    stringstream ss;
    ss<<index;
    return ss.str();
  }
  virtual AstAttribute::OwnershipPolicy
    getOwnershipPolicy() const ROSE_OVERRIDE {
    return CONTAINER_OWNERSHIP;
  }
};

class ConstReporter {
 public:
  virtual ~ConstReporter();
  virtual bool isConst(SgNode* node)=0;
  virtual int getConstInt()=0;
  virtual VariableId getVariableId()=0;
  virtual SgVarRefExp* getVarRefExp()=0;
};

class PStateConstReporter : public ConstReporter {
public:
  PStateConstReporter(const PState* pstate, VariableIdMapping* variableIdMapping);
  bool isConst(SgNode* node);
  int getConstInt();
  VariableId getVariableId();
  SgVarRefExp* getVarRefExp();
private:
  const PState* _pstate;
  VariableIdMapping* _variableIdMapping;
  SgVarRefExp* _varRefExp;
};

class SpecializationConstReporter : public ConstReporter {
public:
  SpecializationConstReporter(VariableIdMapping* variableIdMapping, VariableId var, int constInt);
  virtual bool isConst(SgNode* node);
  virtual int getConstInt();
  virtual VariableId getVariableId();
  virtual SgVarRefExp* getVarRefExp();
private:
  VariableIdMapping* _variableIdMapping;
  VariableId _variableId;
  SgVarRefExp* _varRefExp;
  int _constInt;
};
  
class Specialization {
 public:
  Specialization();

  static void initDiagnostics();
 
  void transformArrayProgram(SgProject* root, Analyzer* analyzer);
  void extractArrayUpdateOperations(Analyzer* ana,
                                    ArrayUpdatesSequence& arrayUpdates,
                                    RewriteSystem& rewriteSystem,
                                    bool useConstExprSubstRule=true
                                    );
  void printUpdateInfos(ArrayUpdatesSequence& arrayUpdates, VariableIdMapping* variableIdMapping);
  void writeArrayUpdatesToFile(ArrayUpdatesSequence& arrayUpdates, string filename, SAR_MODE sarMode, bool performSorting);
  void createSsaNumbering(ArrayUpdatesSequence& arrayUpdates, VariableIdMapping* variableIdMapping);
  // specializes function with name funNameToFind and replace variable of parameter param with constInt
  int specializeFunction(SgProject* project, string funNameToFind, int param, int constInt, VariableIdMapping* variableIdMapping);
  int specializeFunction(SgProject* project, string funNameToFind, int param, int constInt, string varInitName, int initConst, VariableIdMapping* variableIdMapping);
  SgFunctionDefinition* getSpecializedFunctionRootNode() { return _specializedFunctionRootNode; }
  void substituteArrayRefs(ArrayUpdatesSequence& arrayUpdates, VariableIdMapping* variableIdMapping, SAR_MODE sarMode, RewriteSystem& rewriteSystem);
  void setMaxNumberOfExtractedUpdates(long);
  long getMaxNumberOfExtractedUpdates();
  bool dataRaceDetection=false; // temporary flag
 private:
  static Sawyer::Message::Facility logger;
  string iterVarsToString(IterationVariables iterationVars, VariableIdMapping* variableIdMapping);
  int substituteConstArrayIndexExprsWithConst(VariableIdMapping* variableIdMapping, ExprAnalyzer* exprAnalyzer, const EState* estate, SgNode* root);
  VariableId determineVariableIdToSpecialize(SgFunctionDefinition* funDef, int param, VariableIdMapping* variableIdMapping);

  // replaces each use of SgVarRefExp if the corresponding variableId in pstate is constant (with this constant)
  int substituteVariablesWithConst(VariableIdMapping* variableIdMapping, const PState* pstate, SgNode *node);

  // replaces each use of a SgVarRefExp of variableId with constInt.
  int substituteVariablesWithConst(SgNode* node, VariableIdMapping* variableIdMapping, VariableId variableId, int constInt);
  int substituteVarInitWithConst(SgFunctionDefinition* funDef, VariableIdMapping* variableIdMapping, string varInitName, int varInitConstInt);

  // replace each use of a SgVarRefExp according to constReporter
  int substituteVariablesWithConst(SgNode* node, ConstReporter* constReporter);

  SgNode* findDefAssignOfArrayElementUse(SgPntrArrRefExp* useRefNode, ArrayUpdatesSequence& arrayUpdates, ArrayUpdatesSequence::iterator pos, VariableIdMapping* variableIdMapping);
  SgNode* findDefAssignOfUse(SgVarRefExp* useRefNode, ArrayUpdatesSequence& arrayUpdates, ArrayUpdatesSequence::iterator pos, VariableIdMapping* variableIdMapping);
  void attachSsaNumberingtoDefs(ArrayUpdatesSequence& arrayUpdates, VariableIdMapping* variableIdMapping);
  //SgExpressionPtrList& getInitializerListOfArrayVariable(VariableId arrayVar, VariableIdMapping* variableIdMapping);
  string flattenArrayInitializer(SgVariableDeclaration* decl, VariableIdMapping* variableIdMapping);
  void transformArrayAccess(SgNode* node, VariableIdMapping* variableIdMapping);
  
  SgFunctionDefinition* _specializedFunctionRootNode=0;
  long _maxNumberOfExtractedUpdates=-1;
};


#endif
