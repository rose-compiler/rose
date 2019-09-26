#ifndef LOOP_INFO_H
#define LOOP_INFO_H

#include "Labeler.h"
#include "VariableIdMapping.h"
#include "EState.h"

enum IterVarType { ITERVAR_SEQ, ITERVAR_PAR, ITERVAR_UNKNOWN };

struct LoopInfo {
  LoopInfo();
  ~LoopInfo();
  SgForStatement* forStmt;
  SgStatement* initStmt;
  SgExpression* condExpr;
  bool isOmpCanonical;
  IterVarType iterationVarType;
  static CodeThorn::VariableId iterationVariableId(SgForStatement* forStmt, VariableIdMapping* variableIdMapping);
  CodeThorn::VariableId iterationVarId;
  CodeThorn::VariableIdSet outerLoopsVarIds;
  CodeThorn::LabelSet loopLabelSet;
  void computeOuterLoopsVarIds(CodeThorn::VariableIdMapping* variableIdMapping);
  void computeLoopLabelSet(CodeThorn::Labeler* labeler);
  bool isInAssociatedLoop(const CodeThorn::EState* estate);
  std::string toString();
};

typedef vector< LoopInfo > LoopInfoSet;

#endif
