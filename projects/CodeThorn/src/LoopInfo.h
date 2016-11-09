#ifndef LOOP_INFO_H
#define LOOP_INFO_H

#include "Labeler.h"
#include "VariableIdMapping.h"
#include "StateRepresentations.h"

enum IterVarType { ITERVAR_SEQ, ITERVAR_PAR };

struct LoopInfo {
  SPRAY::VariableId iterationVarId;
  IterVarType iterationVarType;
  SgStatement* initStmt;
  SgExpression* condExpr;
  SgForStatement* forStmt;
  SPRAY::VariableIdSet outerLoopsVarIds;
  void computeOuterLoopsVarIds(SPRAY::VariableIdMapping* variableIdMapping);
  void computeLoopLabelSet(SPRAY::Labeler* labeler);
  bool isInAssociatedLoop(const CodeThorn::EState* estate);
  SPRAY::LabelSet loopLabelSet;
  static SPRAY::VariableId iterationVariableId(SgForStatement* forStmt, VariableIdMapping* variableIdMapping);
};

typedef vector< LoopInfo > LoopInfoSet;

#endif
