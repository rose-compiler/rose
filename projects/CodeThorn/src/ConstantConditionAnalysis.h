#ifndef CONSTANT_CONDITION_ANALYSIS_H 
#define CONSTANT_CONDITION_ANALYSIS_H 

//#include "ReadWriteListener.h"
#include "EStateTransferFunctions.h"
#include <map>
#include "BoolLattice.h"

class ConstantConditionAnalysis : public ReadWriteListener {
 public:
  virtual void trueFalseEdgeEvaluation(Edge edge, SingleEvalResult evalResult , const EState* estate);
  typedef std::map <Label,BoolLattice> ConstConditionsMap;
  ConstConditionsMap* getResultMapPtr();
 private:
  ConstConditionsMap constConditions;
};

#endif
