#include "sage3basic.h"
#include "ProgramAbstractionLayer.h"

SPRAY::ProgramAbstractionLayer::ProgramAbstractionLayer()
  :_modeArrayElementVariableId(true),_labeler(0),_variableIdMapping(0) {
}

void SPRAY::ProgramAbstractionLayer::setModeArrayElementVariableId(bool val) {
  _modeArrayElementVariableId=val; 
}

bool SPRAY::ProgramAbstractionLayer::getModeArrayElementVariableId() {
  return _modeArrayElementVariableId;; 
}

void SPRAY::ProgramAbstractionLayer::initialize(SgProject* root) {
  _variableIdMapping=new VariableIdMapping();
  getVariableIdMapping()->setModeVariableIdForEachArrayElement(getModeArrayElementVariableId());
  getVariableIdMapping()->computeVariableSymbolMapping(root);
  _labeler=new Labeler(root);
}

SPRAY::Labeler* SPRAY::ProgramAbstractionLayer::getLabeler(){
  ROSE_ASSERT(_labeler!=0);
  return _labeler;
}

SPRAY::VariableIdMapping* SPRAY::ProgramAbstractionLayer::getVariableIdMapping(){
  ROSE_ASSERT(_variableIdMapping!=0);
  return _variableIdMapping;
}
