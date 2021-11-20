
#include "sage3basic.h"
#include "ConstantConditionAnalysis.h"

void ConstantConditionAnalysis::trueFalseEdgeEvaluation(Edge edge, SingleEvalResult& evalResult , EStatePtr estate) {
  BoolLattice val=constConditions[estate->label()]; // default is bot
  // determine whether all results at this label are true (or false), otherwise top.
  if(val.isBot()) {
    if(evalResult.isTrue())
      constConditions[estate->label()]=BoolLattice(1);
    if(evalResult.isFalse())
      constConditions[estate->label()]=BoolLattice(0);
  } else {
    if((val.isTrue() && evalResult.isFalse()) 
       || (val.isFalse() && evalResult.isTrue())) { 
       constConditions[estate->label()]=BoolLattice(CodeThorn::Top());
    }
  }
  // if evalResult is top in any evaluation the result is top
  if(evalResult.isTop()) {
    constConditions[estate->label()]=BoolLattice(CodeThorn::Top());
  }
}

ConstantConditionAnalysis::ConstConditionsMap*
ConstantConditionAnalysis::getResultMapPtr() {
  return &constConditions;
}
