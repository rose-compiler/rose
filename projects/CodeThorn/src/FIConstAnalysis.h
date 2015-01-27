#ifndef FICONSTANALYSIS_H
#define FICONSTANALYSIS_H

#include "AType.h"
#include <cassert>
#include <map>
#include <set>
#include "VariableIdMapping.h"
#include "Labeler.h"

using namespace std;
using namespace CodeThorn::AType;
using namespace SPRAY;

// does not support -inf, +inf yet
class VariableValueRangeInfo {
public:
  VariableValueRangeInfo(ConstIntLattice min, ConstIntLattice max);
  VariableValueRangeInfo(ConstIntLattice value);
  bool isTop() const { return _width.isTop(); }
  bool isBot() const { return _width.isBot(); }
  bool isEmpty() const { return (_width==0).isTrue(); }
  ConstIntLattice minValue() const { return _min; }
  ConstIntLattice maxValue() const { return _max; }
  int minIntValue() const { assert(_min.isConstInt()); return _min.getIntValue(); }
  int maxIntValue() const { assert(_max.isConstInt()); return _max.getIntValue(); }
  ConstIntLattice width() const { return _width; }
  string toString() const {
    if(isBot())
      return "bot";
    if(isTop())
      return "top";
    return string("[")+_min.toString()+","+_max.toString()+"]";
  }
  void setArraySize(int asize);
  int arraySize();
private:
  ConstIntLattice _width;
  ConstIntLattice _min;
  ConstIntLattice _max;
  int _asize;
};

typedef map<VariableId, set<CppCapsuleConstIntLattice> > VarConstSetMap;

class VariableConstInfo {
public:
  VariableConstInfo(VariableIdMapping* variableIdMapping, VarConstSetMap* map);
  bool isAny(VariableId);
  bool isUniqueConst(VariableId);
  bool isMultiConst(VariableId);
  size_t width(VariableId);
  bool isInConstSet(VariableId varId, int varVal);
  int uniqueConst(VariableId);
  int minConst(VariableId);
  int maxConst(VariableId);
  int arraySize(VariableId);
  bool haveEmptyIntersection(VariableId,VariableId);
  static VariableValueRangeInfo createVariableValueRangeInfo(VariableId varId, VarConstSetMap& map);
  static ConstIntLattice isConstInSet(ConstIntLattice val, set<CppCapsuleConstIntLattice> valSet);
private:
  VariableIdMapping* _variableIdMapping;
  VarConstSetMap* _map;
};

class VariableValuePair {
public:
  VariableValuePair(){}
  VariableValuePair(VariableId varId, ConstIntLattice varValue):varId(varId),varValue(varValue){}
  VariableId varId;
  ConstIntLattice varValue;
  string toString(VariableIdMapping& varIdMapping) {
    string varNameString=varIdMapping.uniqueShortVariableName(varId);
    string varValueString=varValue.toString();
    return varNameString+"="+varValueString;
  }
};

typedef ConstIntLattice EvalValueType;
class FIConstAnalysis {
 public:
  FIConstAnalysis(VariableIdMapping*);

  void runAnalysis(SgProject* root);
  void runAnalysis(SgProject* root, SgFunctionDefinition* mainFunction);
  VariableConstInfo* getVariableConstInfo();
  void attachAstAttributes(Labeler* labeler, string attributeName);

  void setOptionMultiConstAnalysis(bool);
  void writeCvsConstResult(VariableIdMapping& variableIdMapping, string filename);

  VariableIdMapping::VariableIdSet determinedConstantVariables();

  static ConstIntLattice analyzeAssignRhs(SgNode* rhs);
  static bool determineVariable(SgNode* node, VariableId& varId, VariableIdMapping& _variableIdMapping);
  static bool analyzeAssignment(SgAssignOp* assignOp,VariableIdMapping& varIdMapping, VariableValuePair* result);
  VariableValuePair analyzeVariableDeclaration(SgVariableDeclaration* decl,VariableIdMapping& varIdMapping);

  void determineVarConstValueSet(SgNode* node, VariableIdMapping& varIdMapping, VarConstSetMap& map);
  VarConstSetMap computeVarConstValues(SgProject* project, SgFunctionDefinition* mainFunctionRoot, VariableIdMapping& variableIdMapping);

  int performConditionConstAnalysis(Labeler* labeler);
  LabelSet getTrueConditions();
  LabelSet getFalseConditions();
  LabelSet getNonConstConditions();

  void setDetailedOutput(bool);

  // removes all variables that are NOT in this provided set
  void filterVariables(VariableIdSet& variableIdSet);

 private:
  // Expression evaluation functions
  EvalValueType eval(SgExpression* node);
  EvalValueType evalWithMultiConst(SgNode* op, SgVarRefExp* var, EvalValueType val);
  EvalValueType evalWithMultiConst(SgNode* op, SgVarRefExp* lhsVar, SgVarRefExp* rhsVar);
  EvalValueType evalSgBoolValExp(SgExpression* node);
  EvalValueType evalSgAndOp(EvalValueType lhsResult,EvalValueType rhsResult);
  EvalValueType evalSgOrOp(EvalValueType lhsResult,EvalValueType rhsResult);
  EvalValueType evalSgVarRefExp(SgExpression* node);
  EvalValueType evalSgIntVal(SgExpression* node);
  void setVariableConstInfo(VariableConstInfo* varConstInfo);
  bool isConstVal(SgExpression* node);
  bool isRelationalOperator(SgExpression* node);

private:
  VariableIdMapping* global_variableIdMapping;
  VarConstSetMap _varConstSetMap;
  VariableConstInfo* global_variableConstInfo;
  bool option_multiconstanalysis;
  bool detailedOutput;
  set<VariableId> variablesOfInterest;

  LabelSet trueConditions;
  LabelSet falseConditions;
  LabelSet nonConstConditions;
};

#endif
