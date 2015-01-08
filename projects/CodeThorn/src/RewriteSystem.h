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
  void reset();
  std::string toString();
private:
  void init();
};

class RewriteSystem {
 public:
  void rewriteCompoundAssignments(SgNode*& root, VariableIdMapping* variableIdMapping);
  void rewriteAst(SgNode*& root, VariableIdMapping* variableIdMapping, bool rewriteTrace=false, bool ruleAddReorder=false);
  RewriteStatistics getStatistics();
  void resetStatistics();
 public:
  RewriteStatistics dump1_stats;
};


#endif
