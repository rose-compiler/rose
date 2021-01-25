
#include "sage3basic.h"
#include "ConstantConditionAnalysis.h"

void ConstantConditionAnalysis::trueFalseEdgeEvaluation(Edge edge, SingleEvalResultConstInt evalResult , const EState* estate) {
  if(evalResult.isTrue()) {
    constConditions[estate->label()]=true;
    //cout<<"DEBUG: const cond: true  @L"<<estate->label().toString()<<endl;
  } else if(evalResult.isFalse()) {
    constConditions[estate->label()]=false;
    //cout<<"DEBUG: const cond: false @L"<<estate->label().toString()<<endl;
  } else {
    // cannot be determined to be const
  }
}

ConstantConditionAnalysis::ConstConditionsMap*
ConstantConditionAnalysis::getResultMapPtr() {
  return &constConditions;
}
