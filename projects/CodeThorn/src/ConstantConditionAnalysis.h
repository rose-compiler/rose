#ifndef CONSTANT_CONDITION_ANALYSIS_H 
#define CONSTANT_CONDITION_ANALYSIS_H 

//#include "ReadWriteListener.h"
#include "ExprAnalyzer.h"
#include <map>

class ConstantConditionAnalysis : public ReadWriteListener {
 public:
  virtual void trueFalseEdgeEvaluation(Edge edge, SingleEvalResultConstInt evalResult , const EState* estate);
  typedef std::map <Label,bool> ConstConditionsMap;
  ConstConditionsMap* getResultMapPtr();
 private:
  ConstConditionsMap constConditions;
};

#endif
