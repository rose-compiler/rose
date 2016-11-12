#ifndef LOOP_INFO_H
#define LOOP_INFO_H

#include "Labeler.h"
#include "VariableIdMapping.h"
#include "StateRepresentations.h"

enum IterVarType { ITERVAR_SEQ, ITERVAR_PAR, ITERVAR_UNKNOWN };

struct LoopInfo {
  LoopInfo();
  ~LoopInfo();
  SgForStatement* forStmt;
  SgStatement* initStmt;
  SgExpression* condExpr;
  bool isOmpCanonical;
  IterVarType iterationVarType;
  static SPRAY::VariableId iterationVariableId(SgForStatement* forStmt, VariableIdMapping* variableIdMapping);
  SPRAY::VariableId iterationVarId;
  SPRAY::VariableIdSet outerLoopsVarIds;
  SPRAY::LabelSet loopLabelSet;
  void computeOuterLoopsVarIds(SPRAY::VariableIdMapping* variableIdMapping);
  void computeLoopLabelSet(SPRAY::Labeler* labeler);
  bool isInAssociatedLoop(const CodeThorn::EState* estate);
  std::string toString();
};

typedef vector< LoopInfo > LoopInfoSet;

#endif
