#ifndef CPPEXPREVALUATOR_H
#define CPPEXPREVALUATOR_H

#include "VariableIdMapping.h"
#include "PropertyState.h"
#include "NumberIntervalLattice.h"
#include "PointerAnalysisInterface.h"

namespace CodeThorn {
class CppExprEvaluator {
 public:
  CppExprEvaluator(NumberIntervalLattice* d, VariableIdMapping* vim);
  NumberIntervalLattice evaluate(SgNode* node);
  NumberIntervalLattice evaluate(SgNode* node, PropertyState* pstate);
  void setDomain(NumberIntervalLattice* domain);
  void setPropertyState(PropertyState* pstate);
  void setVariableIdMapping(VariableIdMapping* variableIdMapping);
  bool isValid();
  void setShowWarnings(bool warnings);
  void setPointerAnalysis(CodeThorn::PointerAnalysisInterface* pointerAnalysisInterface);
  void setSoundness(bool s);
  void setSkipSelectedFunctionCalls(bool flag);
  bool getSkipSelectedFunctionCalls();
 private:
  // takes the expression of SgSizeOfOp(Exp) and determines its size
  int computeSize(SgSizeOfOp* node);
  bool isExprRootNode(SgNode* node);
  SgNode* findExprRootNode(SgNode* node);
  NumberIntervalLattice* domain;
  VariableIdMapping* variableIdMapping;
  PropertyState* propertyState;
  bool _showWarnings;
  CodeThorn::PointerAnalysisInterface* _pointerAnalysisInterface;
  bool _sound;
  bool _skipSelectedFunctionCalls=false;
};
}
#endif
