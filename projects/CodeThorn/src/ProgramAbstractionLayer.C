#include "sage3basic.h"
#include "ProgramAbstractionLayer.h"

SPRAY::ProgramAbstractionLayer::ProgramAbstractionLayer()
  :_modeArrayElementVariableId(true),_labeler(0),_variableIdMapping(0) {
}

SPRAY::ProgramAbstractionLayer::~ProgramAbstractionLayer() {
}

void SPRAY::ProgramAbstractionLayer::setModeArrayElementVariableId(bool val) {
  _modeArrayElementVariableId=val; 
}

bool SPRAY::ProgramAbstractionLayer::getModeArrayElementVariableId() {
  return _modeArrayElementVariableId;; 
}

void SPRAY::ProgramAbstractionLayer::initialize(SgProject* root, bool computeFunctionIdMapping) {
  _variableIdMapping=new VariableIdMapping();
  getVariableIdMapping()->setModeVariableIdForEachArrayElement(getModeArrayElementVariableId());
  getVariableIdMapping()->computeVariableSymbolMapping(root);
  _labeler=new Labeler(root);
  _functionIdMapping=new FunctionIdMapping();
  if(computeFunctionIdMapping) {
    getFunctionIdMapping()->computeFunctionSymbolMapping(root);
  }
}

SPRAY::Labeler* SPRAY::ProgramAbstractionLayer::getLabeler(){
  ROSE_ASSERT(_labeler!=0);
  return _labeler;
}

SPRAY::VariableIdMapping* SPRAY::ProgramAbstractionLayer::getVariableIdMapping(){
  ROSE_ASSERT(_variableIdMapping!=0);
  return _variableIdMapping;
}

SPRAY::FunctionIdMapping* SPRAY::ProgramAbstractionLayer::getFunctionIdMapping(){
  ROSE_ASSERT(_functionIdMapping!=0);
  return _functionIdMapping;
}
