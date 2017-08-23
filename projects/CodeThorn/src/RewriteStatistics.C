#include "sage3basic.h"
#include "RewriteStatistics.h"

RewriteStatistics RewriteSystem::getRewriteStatistics() {
  return dump1_stats;
}

RewriteStatistics::RewriteStatistics() {
  init();
}

void RewriteStatistics::init() {
  numElimMinusOperator=0;
  numElimAssignOperator=0;
  numAddOpReordering=0;
  numConstantFolding=0;
  numVariableElim=0;
  numArrayUpdates=0;
  numConstExprElim=0;
}
void RewriteStatistics::reset() {
  init();
}

string RewriteStatistics::toString() {
  stringstream ss;
  ss<<"Array updates  : "<<numArrayUpdates<<endl;
  ss<<"Elim minus op  : "<<numElimMinusOperator<<endl;
  ss<<"Elim assign op : "<<numElimAssignOperator<<endl;
  ss<<"Add op reorder : "<<numAddOpReordering<<endl;
  ss<<"Const fold     : "<<numConstantFolding<<endl;
  ss<<"Variable elim  : "<<numVariableElim<<endl;
  ss<<"Const expr elim: "<<numConstExprElim<<endl;
  return ss.str();
}

string RewriteStatistics::toCsvString() {
  stringstream ss;
  ss<<numArrayUpdates
    <<","<<numElimMinusOperator
    <<","<<numElimAssignOperator
    <<","<<numAddOpReordering
    <<","<<numConstantFolding
    <<","<<numVariableElim
    <<","<<numConstExprElim
    ;
  return ss.str();
}
