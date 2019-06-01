#ifndef REWRITESYSTEM_H
#define REWRITESYSTEM_H

#include "VariableIdMapping.h"
#include "Labeler.h"
#include "AstMatching.h"
#include "RewriteStatistics.h"

class RewriteSystem {
 public:
  RewriteSystem();
  void rewriteAst(SgNode*& root, CodeThorn::VariableIdMapping* variableIdMapping, bool ruleAddReorder=false, bool performCompoundAssignmentsElimination=false, bool ruleAlgebraic=false);
  bool getRewriteCondStmt();
  void setRewriteCondStmt(bool);

  RewriteStatistics getStatistics();
  void resetStatistics();
  RewriteStatistics getRewriteStatistics();
  RewriteStatistics* getRewriteStatisticsPtr() { return &_rewriteStatistics; }

  void rewriteCompoundAssignmentsInAst(SgNode* root, CodeThorn::VariableIdMapping* variableIdMapping);
  void rewriteCompoundAssignments(SgNode*& root, CodeThorn::VariableIdMapping* variableIdMapping);
  SgNode* buildRewriteCompoundAssignment(SgNode* root, CodeThorn::VariableIdMapping* variableIdMapping);

  // sorts nodes of operators SgAddOp and SgMultiplyOp according to variantT bottom up
  // shallow sort at each node
  void establishCommutativeOrder(SgNode*& root, CodeThorn::VariableIdMapping* variableIdMapping);

  // transform conditions in 'SgDoWhileStatement', 'SgWhileStatement',
  // 'SgForStatement', 'ifStatement' into SgStatementExpressions that
  // contain the original condition.
  void rewriteCondStmt(SgNode* root);

  // applies rewriteConditionsInStmtExpression to entire AST
  void rewriteCondStmtInAst(SgNode* root);

  static void initDiagnostics();
  void setTrace(bool);
  bool getTrace();

  void setRuleCommutativeSort(bool flag) { ruleCommutativeSort=flag; }
  bool getRuleCommutativeSort() { return ruleCommutativeSort; }

 private:
  static Sawyer::Message::Facility logger;
  bool _trace;
  bool _rewriteCondStmt;
  // sets valueString to empty string in SgFloatVal, SgDoubleVal, SgLongDoubleVal
  void normalizeFloatingPointNumbersForUnparsing(SgNode*& root);
  RewriteStatistics _rewriteStatistics;
  bool ruleCommutativeSort=false;
};


#endif
