#include "PointerAnalysisInterface.h"

SPRAY::PointerAnalysisInterface::PointerAnalysisInterface() {
}

SPRAY::PointerAnalysisInterface::~PointerAnalysisInterface() {
}

SPRAY::PointerAnalysisEmptyImplementation::PointerAnalysisEmptyImplementation(VariableIdMapping* vim) {
  _variableIdMapping=vim;
}

void
SPRAY::PointerAnalysisEmptyImplementation::initialize() {
  // nothing to be done
}

void 
SPRAY::PointerAnalysisEmptyImplementation::run() {
  ROSE_ASSERT(_variableIdMapping);
  // nothing to be done
}

SPRAY::VariableIdSet SPRAY::PointerAnalysisEmptyImplementation::getModByPointer() {
  ROSE_ASSERT(_variableIdMapping);
  return _variableIdMapping->getVariableIdSet();
}
