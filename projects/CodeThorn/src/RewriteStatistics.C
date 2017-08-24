#include "sage3basic.h"
#include "RewriteStatistics.h"

using namespace std;

RewriteStatistics::RewriteStatistics() {
  init();
}

void RewriteStatistics::init() {
  numElimMinusOperator=0;
  numElimCompoundAssignOperator=0;
  numAddOpReordering=0;
  numConstantFolding=0;
  numVariableElim=0;
  numArrayUpdates=0;
  numConstExprElim=0;
  numUnaryMinusToBinaryMinusConversion=0; // E1+(-E2) => E1-E2, (-E2)+E1 => E1-E2
  numBinaryAndUnaryMinusToBinaryAddConversion=0; // E1-(-E2) => E1+E2
  numBinaryAndUnaryMinusToBinarySubConversion=0; // (-E1)-E2 => -(E1+E2)
  numMultiplyMinusOneConversion=0; // E*(-1) => -E, (-1)*E => -E
  numZeroSubEConversion=0; // 0-E=>-E
  numAddZeroElim=0; // E+0=>E, 0+E=>E
  numMultiplyOneElim=0; // E*1=>E, 1*E=>E
  numCommutativeSwap=0;
}
void RewriteStatistics::reset() {
  init();
}

string RewriteStatistics::toString() {
  stringstream ss;
  ss<<"Array updates    : "<<numArrayUpdates<<endl;
  ss<<"Elim minus op    : "<<numElimMinusOperator<<endl;
  ss<<"Elim assign op   : "<<numElimCompoundAssignOperator<<endl;
  ss<<"Add op reorder   : "<<numAddOpReordering<<endl;
  ss<<"Const fold       : "<<numConstantFolding<<endl;
  ss<<"Variable elim    : "<<numVariableElim<<endl;
  ss<<"Const expr elim  : "<<numConstExprElim<<endl;
  ss<<"E1+(-E2) => E1-E2: "<<numUnaryMinusToBinaryMinusConversion<<endl; // E1+(-E2) => E1-E2, (-E2)+E1 => E1-E2
  ss<<"E1-(-E2) => E1+E2: "<<numBinaryAndUnaryMinusToBinaryAddConversion<<endl; // E1-(-E2) => E1+E2
  ss<<"-E1-E2=>-(E1+E2) : "<<numBinaryAndUnaryMinusToBinarySubConversion<<endl; // (-E1)-E2 => -(E1+E2)
  ss<<"E*(-1) => -E     : "<<numMultiplyMinusOneConversion<<endl; // E*(-1) => -E, (-1)*E => -E
  ss<<"0-E => -E        : "<<numZeroSubEConversion<<endl; // 0-E=>-E
  ss<<"E+0 => E         : "<<numAddZeroElim<<endl; // E+0=>E, 0+E=>E
  ss<<"E*1 => E         : "<<numMultiplyOneElim<<endl; // E*1=>E, 1*E=>E
  ss<<"Commutative Swap : "<<numCommutativeSwap<<endl;
  return ss.str();
}

string RewriteStatistics::toCsvString() {
  stringstream ss;
  ss<<"U:"<<numArrayUpdates
    <<","<<numElimMinusOperator
    <<","<<numElimCompoundAssignOperator
    <<","<<numAddOpReordering
    <<","<<numConstantFolding
    <<","<<numVariableElim
    <<","<<numConstExprElim
    <<",New:"<<numUnaryMinusToBinaryMinusConversion // E1+(-E2) => E1-E2, (-E2)+E1 => E1-E2
    <<","<<numBinaryAndUnaryMinusToBinaryAddConversion // E1-(-E2) => E1+E2
    <<","<<numBinaryAndUnaryMinusToBinarySubConversion // (-E1)-E2 => -(E1+E2)
    <<","<<numMultiplyMinusOneConversion // E*(-1) => -E, (-1)*E => -E
    <<","<<numZeroSubEConversion // 0-E=>-E
    <<","<<numAddZeroElim // E+0=>E, 0+E=>E
    <<","<<numMultiplyOneElim // E*1=>E, 1*E=>E
    <<",C:"<<numCommutativeSwap
    ;
  return ss.str();
}
