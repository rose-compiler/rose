#include "FIPointerAnalysis.h"
#include "AstUtility.h"

CodeThorn::FIPointerAnalysis::FIPointerAnalysis(VariableIdMapping* vim, FunctionIdMapping* fim, SgProject* project):_variableIdMapping(vim), _functionIdMapping(fim), _astRoot(project) {
}

CodeThorn::FIPointerAnalysis::~FIPointerAnalysis() {
  delete _fipi;
}
void CodeThorn::FIPointerAnalysis::run() {
  _fipi->collectInfo();
  _modVariableIds=_fipi->getAddressTakenVariables();
  _modFunctionIds=_fipi->getAddressTakenFunctions();
}

void CodeThorn::FIPointerAnalysis::initialize() {
  VariableIdSet usedVarsInProgram = AstUtility::usedVariablesInsideFunctions(_astRoot, _variableIdMapping);
  _fipi=new FlowInsensitivePointerInfo(_astRoot, *_variableIdMapping, *_functionIdMapping, usedVarsInProgram);
  //cout<<"DEBUG: _modVariableIds.size(): "<<_modVariableIds.size()<<endl;
}

CodeThorn::VariableIdSet CodeThorn::FIPointerAnalysis::getModByPointer() {
  return getAddressTakenVariables();
}

CodeThorn::VariableIdSet CodeThorn::FIPointerAnalysis::getAddressTakenVariables() {
  return _modVariableIds;
}

CodeThorn::FunctionIdSet CodeThorn::FIPointerAnalysis::getAddressTakenFunctions() {
  return _modFunctionIds;
}

CodeThorn::FlowInsensitivePointerInfo* CodeThorn::FIPointerAnalysis::getFIPointerInfo() {
  return _fipi;
}
