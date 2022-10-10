#ifndef CONSTANT_CONDITION_ANALYSIS_H 
#define CONSTANT_CONDITION_ANALYSIS_H 

#include <map>

#include "EStateTransferFunctions.h"
#include "ReadWriteListener.h"
#include "BoolLattice.h"

class ConstantConditionAnalysis : public CodeThorn::ReadWriteListener {
 public:
  void trueFalseEdgeEvaluation(CodeThorn::Edge edge, CodeThorn::SingleEvalResult& evalResult , CodeThorn::EStatePtr estate) override;
  typedef std::map <CodeThorn::Label,CodeThorn::BoolLattice> ConstConditionsMap;
  ConstConditionsMap* getResultMapPtr();
 private:
  ConstConditionsMap constConditions;
};

#endif
