#ifndef REWRITESYSTEM_H
#define REWRITESYSTEM_H

#include "VariableIdMapping.h"
#include "Labeler.h"
#include "AstMatching.h"

struct RewriteStatistics {
  RewriteStatistics();
  int numElimMinusOperator;
  int numElimAssignOperator;
  int numAddOpReordering;
  int numConstantFolding;
  int numVariableElim;
  int numArrayUpdates; // number of array updates (i.e. assignments)
  int numConstExprElim; // number of const-expr found and substituted by constant (new rule, includes variables)
  // resets all statistics counters to 0.
  void reset();
  std::string toString();
  // create a comma separated value string
  std::string toCsvString();
private:
  void init();
};

class RewriteSystem {
 public:
  RewriteSystem();
  void rewriteAst(SgNode*& root, SPRAY::VariableIdMapping* variableIdMapping, bool rewriteTrace=false, bool ruleAddReorder=false, bool performCompoundAssignmentsElimination=false);
  bool getRewriteCondStmt();
  void setRewriteCondStmt(bool);

  RewriteStatistics getStatistics();
  void resetStatistics();
  RewriteStatistics getRewriteStatistics();
  RewriteStatistics* getRewriteStatisticsPtr() { return &dump1_stats; }

  void rewriteCompoundAssignmentsInAst(SgNode* root, SPRAY::VariableIdMapping* variableIdMapping);
  void rewriteCompoundAssignments(SgNode*& root, SPRAY::VariableIdMapping* variableIdMapping);
  SgNode* buildRewriteCompoundAssignment(SgNode* root, SPRAY::VariableIdMapping* variableIdMapping);

  // transform conditions in 'SgDoWhileStatement', 'SgWhileStatement',
  // 'SgForStatement', 'ifStatement' into SgStatementExpressions that
  // contain the original condition.
  void rewriteCondStmt(SgNode* root);

  // applies rewriteConditionsInStmtExpression to entire AST
  void rewriteCondStmtInAst(SgNode* root);

  static void initDiagnostics();

 private:
  static Sawyer::Message::Facility logger;

  bool _rewriteCondStmt;
  // sets valueString to empty string in SgFloatVal, SgDoubleVal, SgLongDoubleVal
  void normalizeFloatingPointNumbersForUnparsing(SgNode*& root);
  RewriteStatistics dump1_stats;
};


#endif
