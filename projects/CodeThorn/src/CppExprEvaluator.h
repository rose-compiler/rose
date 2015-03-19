#ifndef CPPEXPREVALUATOR_H
#define CPPEXPREVALUATOR_H

#include "VariableIdMapping.h"
#include "PropertyState.h"
#include "NumberIntervalLattice.h"

namespace SPRAY {
class CppExprEvaluator {
 public:
  CppExprEvaluator(NumberIntervalLattice* d, VariableIdMapping* vim);
  NumberIntervalLattice evaluate(SgNode* node);
  NumberIntervalLattice evaluate(SgNode* node, PropertyState* pstate);
  void setDomain(NumberIntervalLattice* domain);
  void setPropertyState(PropertyState* pstate);
  void setVariableIdMapping(VariableIdMapping* variableIdMapping);
  bool isValid();
 private:
  NumberIntervalLattice* domain;
  VariableIdMapping* variableIdMapping;
  PropertyState* propertyState;
};
}
#endif
