#ifndef CONSTANT_CONDITION_ANALYSIS_H 
#define CONSTANT_CONDITION_ANALYSIS_H 

#include <map>

#include "EStateTransferFunctions.h"
#include "ReadWriteListener.h"
#include "BoolLattice.h"

class ConstantConditionAnalysis : public ReadWriteListener {
 public:
  void trueFalseEdgeEvaluation(Edge edge, SingleEvalResult& evalResult , EStatePtr estate) override;
  typedef std::map <Label,BoolLattice> ConstConditionsMap;
  ConstConditionsMap* getResultMapPtr();
 private:
  ConstConditionsMap constConditions;
};

#endif
