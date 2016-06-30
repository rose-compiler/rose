#include "FIPointerAnalysis.h"
#include "AnalysisAbstractionLayer.h"

SPRAY::FIPointerAnalysis::FIPointerAnalysis(VariableIdMapping* vim, FunctionIdMapping* fim, SgProject* project):_variableIdMapping(vim), _functionIdMapping(fim), _astRoot(project) {
}

SPRAY::FIPointerAnalysis::~FIPointerAnalysis() {
  delete _fipi;
}
void SPRAY::FIPointerAnalysis::run() {
  _fipi->collectInfo();
  _modVariableIds=_fipi->getMemModByPointer();
}

void SPRAY::FIPointerAnalysis::initialize() {
  VariableIdSet usedVarsInProgram = AnalysisAbstractionLayer::usedVariablesInsideFunctions(_astRoot, _variableIdMapping);
  _fipi=new FlowInsensitivePointerInfo(_astRoot, *_variableIdMapping, *_functionIdMapping, usedVarsInProgram);
  //cout<<"DEBUG: _modVariableIds.size(): "<<_modVariableIds.size()<<endl;
}

SPRAY::VariableIdSet SPRAY::FIPointerAnalysis::getModByPointer() {
  return _modVariableIds;
}

SPRAY::FlowInsensitivePointerInfo* SPRAY::FIPointerAnalysis::getFIPointerInfo() {
  return _fipi;
}
