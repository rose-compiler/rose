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
