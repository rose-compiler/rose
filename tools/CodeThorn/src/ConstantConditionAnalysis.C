
#include "sage3basic.h"
#include "ConstantConditionAnalysis.h"

using namespace CodeThorn;

void ConstantConditionAnalysis::trueFalseEdgeEvaluation(Edge /*edge*/, SingleEvalResult& evalResult , EStatePtr estate) {
  BoolLattice newVal;
  if(evalResult.isTrue())
    newVal=BoolLattice(1);
  else if(evalResult.isFalse())
    newVal=BoolLattice(0);
  else
    newVal=BoolLattice(CodeThorn::Top());

  BoolLattice oldVal=constConditions[estate->label()];
  // combine old with new value (boolean lattice). If it always evaluates to True, it remains True. Analogous for False; otherwise Top (=unknown)
  constConditions[estate->label()]=oldVal.lub(newVal);

}

ConstantConditionAnalysis::ConstConditionsMap*
ConstantConditionAnalysis::getResultMapPtr() {
  return &constConditions;
}
