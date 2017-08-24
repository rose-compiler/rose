struct RewriteStatistics {
  RewriteStatistics();
  int numElimMinusOperator;
  int numElimCompoundAssignOperator;
  int numAddOpReordering;
  int numConstantFolding;
  int numVariableElim;
  int numArrayUpdates; // number of array updates (i.e. assignments)
  int numConstExprElim; // number of const-expr found and substituted by constant (new rule, includes variables)
  // resets all statistics counters to 0.
  int numUnaryMinusToBinaryMinusConversion=0; // E1+(-E2) => E1-E2, (-E2)+E1 => E1-E2
  int numBinaryAndUnaryMinusToBinaryAddConversion=0; // E1-(-E2) => E1+E2
  int numBinaryAndUnaryMinusToBinarySubConversion=0; // (-E1)-E2 => -(E1+E2)
  int numMultiplyMinusOneConversion=0; // E*(-1) => -E, (-1)*E => -E
  int numZeroSubEConversion=0; // 0-E=>-E
  // missing: E-0=>E
  int numAddZeroElim=0; // E+0=>E, 0+E=>E
  int numMultiplyOneElim=0; // E*1=>E, 1*E=>E
  int numCommutativeSwap=0;
  void reset();
  std::string toString();
  // create a comma separated value string
  std::string toCsvString();
private:
  void init();
};
