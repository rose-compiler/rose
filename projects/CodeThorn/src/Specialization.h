#ifndef SPECIALIZATION_H
#define SPECIALIZATOIN_H

#include "VariableIdMapping.h"
#include "StateRepresentations.h"
#include "ArrayElementAccessData.h"
#include "Analyzer.h"
#include "ExprAnalyzer.h"
#include "RewriteSystem.h"
#include <iostream>

using namespace std;
using namespace SPRAY;
using namespace CodeThorn;

enum IterVarType { ITERVAR_SEQ, ITERVAR_PAR };
struct LoopInfo {
  VariableId iterationVarId;
  IterVarType iterationVarType;
  SgStatement* initStmt;
  SgExpression* condExpr;
  SgForStatement* forStmt;
  void computeLoopLabelSet(Labeler* labeler);
  bool isInAssociatedLoop(const EState* estate);
  LabelSet loopLabelSet;
};

typedef vector< pair< VariableId, IterVarType> > IterationVariables;
typedef vector< LoopInfo > LoopInfoSet;

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

struct ReadWriteData {
  ArrayElementAccessDataSet writeArrayAccessSet;
  VariableIdSet writeVarIdSet;
  ArrayElementAccessDataSet readArrayAccessSet;
  VariableIdSet readVarIdSet;
};

typedef vector<int> IndexVector;
typedef map<IndexVector,ReadWriteData> IndexToReadWriteDataMap;

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
  void transformArrayProgram(SgProject* root, Analyzer* analyzer);
  void extractArrayUpdateOperations(Analyzer* ana,
                                    ArrayUpdatesSequence& arrayUpdates,
                                    RewriteSystem& rewriteSystem,
                                    bool useConstExprSubstRule=true
                                    );
  // computes number of race conditions in update sequence (0:OK, >0:race conditions exist).
  int verifyUpdateSequenceRaceConditions(LoopInfoSet loopInfoSet, ArrayUpdatesSequence& arrayUpdates, VariableIdMapping* variableIdMapping);
  void printUpdateInfos(ArrayUpdatesSequence& arrayUpdates, VariableIdMapping* variableIdMapping);
  void writeArrayUpdatesToFile(ArrayUpdatesSequence& arrayUpdates, string filename, SAR_MODE sarMode, bool performSorting);
  void createSsaNumbering(ArrayUpdatesSequence& arrayUpdates, VariableIdMapping* variableIdMapping);
  // specializes function with name funNameToFind and replace variable of parameter param with constInt
  int specializeFunction(SgProject* project, string funNameToFind, int param, int constInt, VariableIdMapping* variableIdMapping);
  SgFunctionDefinition* getSpecializedFunctionRootNode() { return _specializedFunctionRootNode; }

 private:
  string iterVarsToString(IterationVariables iterationVars, VariableIdMapping* variableIdMapping);
  int substituteConstArrayIndexExprsWithConst(VariableIdMapping* variableIdMapping, ExprAnalyzer* exprAnalyzer, const EState* estate, SgNode* root);
  VariableId determineVariableIdToSpecialize(SgFunctionDefinition* funDef, int param, VariableIdMapping* variableIdMapping);

  // replaces each use of SgVarRefExp if the corresponding variableId in pstate is constant (with this constant)
  int substituteVariablesWithConst(VariableIdMapping* variableIdMapping, const PState* pstate, SgNode *node);

  // replaces each use of a SgVarRefExp of variableId with constInt.
  int substituteVariablesWithConst(SgNode* node, VariableIdMapping* variableIdMapping, VariableId variableId, int constInt);

  // replace each use of a SgVarRefExp according to constReporter
  int substituteVariablesWithConst(SgNode* node, ConstReporter* constReporter);

  bool isAtMarker(Label lab, const EState* estate);
  SgNode* findDefAssignOfArrayElementUse(SgPntrArrRefExp* useRefNode, ArrayUpdatesSequence& arrayUpdates, ArrayUpdatesSequence::iterator pos, VariableIdMapping* variableIdMapping);
  SgNode* findDefAssignOfUse(SgVarRefExp* useRefNode, ArrayUpdatesSequence& arrayUpdates, ArrayUpdatesSequence::iterator pos, VariableIdMapping* variableIdMapping);
  void attachSsaNumberingtoDefs(ArrayUpdatesSequence& arrayUpdates, VariableIdMapping* variableIdMapping);
  void substituteArrayRefs(ArrayUpdatesSequence& arrayUpdates, VariableIdMapping* variableIdMapping, SAR_MODE sarMode);
  //SgExpressionPtrList& getInitializerListOfArrayVariable(VariableId arrayVar, VariableIdMapping* variableIdMapping);
  string flattenArrayInitializer(SgVariableDeclaration* decl, VariableIdMapping* variableIdMapping);
  void transformArrayAccess(SgNode* node, VariableIdMapping* variableIdMapping);

  SgFunctionDefinition* _specializedFunctionRootNode;
};

#endif
