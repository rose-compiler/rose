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
  void rewriteCompoundAssignments(SgNode*& root, SPRAY::VariableIdMapping* variableIdMapping);
  void rewriteAst(SgNode*& root, SPRAY::VariableIdMapping* variableIdMapping, bool rewriteTrace=false, bool ruleAddReorder=false, bool performCompoundAssignmentsElimination=false);
  RewriteStatistics getStatistics();
  void resetStatistics();
  RewriteStatistics getRewriteStatistics();
 private:
  RewriteStatistics dump1_stats;
};


#endif
